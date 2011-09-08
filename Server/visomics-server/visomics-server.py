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
