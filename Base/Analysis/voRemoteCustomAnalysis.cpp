/*=========================================================================

  Program: Visomics

  Copyright (c) Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QTimer>
#include <QtGui/QMessageBox>
#include <QtNetwork/QAuthenticator>


// QtPropertyBrowser includes
#include <QtVariantProperty>
#include <QtVariantPropertyManager>

// Visomics includes
#include "voRemoteCustomAnalysis.h"
#include "voDataObject.h"
#include "voInputFileDataObject.h"
#include "voTableDataObject.h"
#include "voUtils.h"
#include "vtkExtendedTable.h"
#include "voCustomAnalysisInformation.h"

// VTK includes
#include <vtkArrayData.h>
#include <vtkCompositeDataIterator.h>
#include <vtkDoubleArray.h>
#include <vtkGraph.h>
#include <vtkMultiPieceDataSet.h>
#include <vtkNew.h>
#include <vtkRCalculatorFilter.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTableToGraph.h>
#include <vtkTree.h>
#include <vtkInformation.h>
#include <vtkInformationKey.h>
#include <vtkInformationStringKey.h>
#include <vtkInformationIntegerKey.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkTreeWriter.h>
#include <vtkTableWriter.h>
#include <vtkTreeReader.h>
#include <vtkTableReader.h>

// JsonCpp
#include <jsoncpp.cpp>

#include <string>


// --------------------------------------------------------------------------
// voRemoteCustomAnalysis methods

// --------------------------------------------------------------------------
voRemoteCustomAnalysis::voRemoteCustomAnalysis(QObject* newParent):
    Superclass(newParent), m_credentialsProvided(false),
    m_networkManager(new QNetworkAccessManager(this))
{

  connect(m_networkManager,
          SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
          this,
          SLOT(provideCredentials(QNetworkReply *, QAuthenticator *)));
}

// --------------------------------------------------------------------------
voRemoteCustomAnalysis::~voRemoteCustomAnalysis()
{
}
// --------------------------------------------------------------------------
bool voRemoteCustomAnalysis::execute()
{
  // First request the connection details
  if (!this->requestConnectionDetails())
    {
    emit error("No connection details provided for remote analysis");
    }

  vtkSmartPointer<vtkExtendedTable> extendedTable;
  vtkNew<vtkMultiBlockDataSet> inputs;

  Json::Value taskRequest;
  taskRequest["name"] = this->information()->name().toStdString();
  taskRequest["inputs"] = Json::Value(Json::arrayValue);

  int index = 0;
  foreach(voCustomAnalysisData *input, this->information()->inputs())
    {
    Json::Value &inputValue = taskRequest["inputs"][Json::ArrayIndex(index)];
    inputValue["name"] = input->name().toStdString();


    vtkDataObject *data;

    if (input->type() == "Table")
      {
      inputValue["type"] = "Table";
      extendedTable = this->getInputTable(index);
      data = extendedTable->GetInputData();
      vtkNew<vtkTableWriter> writer;
      writer->SetWriteToOutputString(1);
      writer->SetInputData(data);
      writer->Update();
      inputValue["data"] = writer->GetOutputStdString();
      }
    else if(input->type() == "Tree")
      {
      data =
        vtkTree::SafeDownCast(this->input(index)->dataAsVTKDataObject());

      vtkNew<vtkTreeWriter> writer;
      writer->SetWriteToOutputString(1);
      writer->SetInputData(data);
      writer->Update();
      inputValue["type"] = "Tree";
      inputValue["data"] = writer->GetOutputStdString();

      if (!data)
        {
        emit error("Input Tree is Null");
        return false;
        }
      }
    else
      {
      emit error(tr("Unsupported input type: %s").arg(input->type()));
      return false;
      }
    ++index;
    }

  vtkNew<vtkMultiBlockDataSet> outputs;

  index = 0;
  taskRequest["outputs"] = Json::Value(Json::arrayValue);
  foreach(voCustomAnalysisData *output, this->information()->outputs())
    {

    Json::Value &outputValue = taskRequest["outputs"][Json::ArrayIndex(index)];

    if (output->type() != "Table" && output->type() != "Tree")
    {
    emit error(tr("Unsupported output type: %s").arg(output->type()));
    return false;
    }

    outputValue["name"] = output->name().toStdString();
    outputValue["type"] = output->type().toStdString();
    ++index;
    }

  // replace each parameter in the script with its actual value
  QString script = this->information()->script();
  foreach(voCustomAnalysisParameter *parameter, this->information()->parameters())
    {
    QString type = parameter->type();
    QString name = parameter->name();
    QString parameterValue;
    if (type == "Integer")
      {
      parameterValue = QString::number(this->integerParameter(name));
      }
    else if (type == "Double")
      {
      parameterValue = QString::number(this->doubleParameter(name));
      }
    else if (type == "Enum")
      {
      parameterValue = this->enumParameter(name);
      }
    else if (type == "String")
      {
      parameterValue = QString("\"%1\"").arg(this->stringParameter(name));
      }
    else
      {
      emit error(tr("Unsupported parameter type in voCustomAnalysis: %s").arg(type));
      return false;
      }
    script.replace(name, parameterValue);
    }

  taskRequest["script"] = script.toStdString();

  Json::StyledWriter writer;
  QString requestBody = QString::fromStdString(writer.write(taskRequest));
  QString postUrl = m_baseUrl + "tasks/celery/visomics/vtk/r";

  QUrl url(postUrl);
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  connect(m_networkManager, SIGNAL(finished(QNetworkReply *)),
          this, SLOT(handleReply(QNetworkReply *)));

  QNetworkReply *reply = m_networkManager->post(request, requestBody.toLocal8Bit());

  if (reply->error() != QNetworkReply::NoError)
    {
    emit error(tr("POST error: %1").arg(reply->errorString()));
    return false;
    }

  return true;
}

void voRemoteCustomAnalysis::handleReply(QNetworkReply *reply)
{
  if (reply->error())
    {
    emit error(QObject::tr("Unable to submit remote task: %1\n").arg(reply->errorString()));
    return;
    }

  QString content = reply->readAll();

  Json::Value root;
  Json::Reader reader;

  if (!reader.parse(content.toStdString(), root))
    {
    emit error("Unable to parse JSON POST response");
    return;
    }

  m_taskId = QString::fromStdString(root["id"].asString());

  emit analysisSubmitted();

  monitorStatus();
}

void voRemoteCustomAnalysis::monitorStatus()
{

  QString statusUrl = tr("%1tasks/celery/%2/status").arg(m_baseUrl)
      .arg(m_taskId);

  QUrl url(statusUrl);
  QNetworkRequest request(url);

  disconnect(m_networkManager, SIGNAL(finished(QNetworkReply *)), 0, 0);
  connect(m_networkManager, SIGNAL(finished(QNetworkReply *)),
          this, SLOT(handleStatusReply(QNetworkReply *)));

  m_networkManager->get(request);
}

void voRemoteCustomAnalysis::handleStatusReply(QNetworkReply *reply)
{
  if (reply->error())
    {
    emit error(QObject::tr("Unable to retrieve remote task status: \n") + reply->errorString());
    return;
    }

  QString content = reply->readAll();

  Json::Value root;
  Json::Reader reader;

  if (!reader.parse(content.toStdString(), root))
    {
    emit error("Unable to parse JSON status response");
    return;
    }

  std::string status = root["status"].asString();

  if (status == "FAILURE")
    {
    emit error(
        tr("Remote R job failed:\n%1")
          .arg(QString::fromStdString(root["message"].asString())));
    return;
    }
  else if (status == "SUCCESS")
    {
    QString resultUrl = tr("%1tasks/celery/%2/result").arg(m_baseUrl)
        .arg(m_taskId);
    QUrl url(resultUrl);
    QNetworkRequest request(url);

    disconnect(m_networkManager, SIGNAL(finished(QNetworkReply *)), 0, 0);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(handleResultReply(QNetworkReply *)));

    m_networkManager->get(request);
    }
  else
    {
    QTimer::singleShot(500, this, SLOT(monitorStatus()));
    }
}

void voRemoteCustomAnalysis::handleResultReply(QNetworkReply *reply)
{
  if (reply->error())
    {
    emit error("Call to remote server failed");
    return;
    }

  QString content = reply->readAll();

  Json::Value root;
  Json::Reader reader;

  if (!reader.parse(content.toStdString(), root))
    {
    emit error("Unable to parse JSON response");
    return;
    }

  Json::Value outputs = root["result"]["output"];
  for (int index=0; index<outputs.size(); index++)
    {
    Json::Value output = outputs[index];
    std::string type = output["type"].asString();
    QString name = QString::fromStdString(output["name"].asString());
    std::string inputString = output["data"].asString();

    if (type == "Table")
      {
      vtkNew<vtkTableReader> reader;
      reader->SetReadFromInputString(1);
      reader->SetInputString(inputString);
      reader->Update();

      this->setOutput(name,
                  new voTableDataObject(name, reader->GetOutput(), true));
      }
    else if (type == "Tree")
      {
      vtkNew<vtkTreeReader> reader;
      reader->SetReadFromInputString(1);
      reader->SetInputString(inputString);
      reader->Update();

      this->setOutput(name,
              new voInputFileDataObject(name, reader->GetOutput()));
      }
    else
      {
      emit error("Unsupported output type");
      }
    }

  emit complete();
}

bool voRemoteCustomAnalysis::requestConnectionDetails()
{
  QUrl connectionDetails;
  emit urlRequired(&connectionDetails);

  if (!connectionDetails.isEmpty())
    {

    m_user = connectionDetails.userName();
    m_password = connectionDetails.password();
    connectionDetails.setUserName("");
    connectionDetails.setPassword("");

    m_baseUrl = connectionDetails.toString();

    if (!m_baseUrl.endsWith('/'))
      m_baseUrl = m_baseUrl + "/";

    return true;
    }
  else
    {
    return false;
    }
}

void voRemoteCustomAnalysis::provideCredentials(QNetworkReply *reply,
    QAuthenticator *auth)
{
  if (m_credentialsProvided)
    {
    // Inform driver that credentials where bad
    emit invalidCredentials();
    // Try again, if user doesn't provide an new credentials cancel the analysis
    if (!requestConnectionDetails()) {
      // Cancel the analysis with no error message
      emit error("");
    }
    m_credentialsProvided = false;
    }

  auth->setUser(m_user);
  auth->setPassword(m_password);
  m_credentialsProvided = true;
}
