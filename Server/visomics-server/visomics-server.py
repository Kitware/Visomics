"""

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

"""

# Visomics web server

import BaseHTTPServer, urlparse, os, sys
import kegg

# --------------------------------------------------------------------------
class MyHandler(BaseHTTPServer.BaseHTTPRequestHandler):
  def do_GET(self):
    url = urlparse.urlsplit(self.path)
    databaseType, queryType = os.path.split(url.path)
    queryList = urlparse.parse_qsl(url.query)
    # Handle query
    if databaseType == '/kegg':
      success, responseData, responseType = kegg.handleQuery(queryType, queryList)
    else: # invalid databaseType
      success = False
    # Send response
    if success:
      self.send_response(200) # 200 OK
      self.send_header('Content-type', responseType)
      self.end_headers()
      self.wfile.write(responseData)
    else:
      self.send_response(400) # 400 Bad Request

# --------------------------------------------------------------------------
def main():
  if len(sys.argv) == 2:
    portNum = int(sys.argv[1])
  else:
    portNum = 8090
  kegg.init()
  try:
    server = BaseHTTPServer.HTTPServer(('', portNum), MyHandler)
    print 'Start HTTP server...'
    server.serve_forever()
  except KeyboardInterrupt:
    print '^C received, shutting down server'
    server.socket.close()

if __name__ == '__main__':
  main()
