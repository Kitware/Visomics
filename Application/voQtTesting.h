

#include <QWidget>

class pqTestUtility;

class voQtTesting : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  voQtTesting(QWidget * newParent = 0);
  ~voQtTesting();
  void playTest(QString filename);

public slots:
  void startRecording();
  void stopRecording();
  void play();

private:
  Q_DISABLE_COPY(voQtTesting)
  pqTestUtility *TestUtility;
};
