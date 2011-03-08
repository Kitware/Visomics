# Visomics web server
#
# To run:
#   sudo python webserver.py
#
# To stop:
#   Control-C
#
# To reset database cache:
#   rm *.sqlite
#
# Retrieve a list of KEGG compounds for a term:
#   http://<server-name>:86/kegg-compound?term=<compound-name>
# Sample result:
# [
#   {
#     "id": "C00064",
#     "names": [
#       "L-Glutamine",
#       "L-2-Aminoglutaramic acid"
#     ]
#   },
#   {
#     "id": "C00303",
#     "names": [
#       "Glutamine",
#       "2-Aminoglutaramic acid"
#     ]
#   }
# ]
#
# Retrieve a list of KEGG pathways for a term:
#   http://<server-name>:86/kegg-pathway?term=<compound-name>
# Sample result:
# [
#   {
#     "id": "ko00230",
#     "name": "Purine metabolism"
#   },
#   {
#     "id": "ko00240",
#     "name": "Pyrimidine metabolism"
#   }
# ]

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import urlparse, cgi, json
import kegg

class MyHandler(BaseHTTPRequestHandler):
  def do_GET(self):
    self.send_response(200)
    self.send_header('Content-type',	'application/json')
    self.end_headers()
    s = urlparse.urlsplit(self.path)
    q = cgi.parse_qs(s.query)
    res = {}
    if s.path == '/kegg-compound':
      res = kegg.compound_search(q.get('term', ['glucose'])[0])
    if s.path == '/kegg-pathway':
      res = kegg.pathway_search(q.get('term', ['glucose'])[0])
    self.wfile.write(json.dumps(res, indent=2))

def main():
  try:
    server = HTTPServer(('', 86), MyHandler)
    print 'started httpserver...'
    server.serve_forever()
  except KeyboardInterrupt:
    print '^C received, shutting down server'
    server.socket.close()

if __name__ == '__main__':
  main()
