import urllib, urllib2
import sys
import json
import sqlite3

db = "kegg.sqlite"

def set_compound_alias(term, compound_id):
  conn = sqlite3.connect(db)
  conn.text_factory = str
  conn.row_factory = sqlite3.Row
  cur = conn.cursor()

  cur.execute('CREATE TABLE IF NOT EXISTS compound_alias (term TEXT, compound_id TEXT)')
  cur.execute('INSERT INTO compound_alias (term, compound_id) VALUES (?, ?)', (term, compound_id))
  conn.commit()

  cur.close()
  conn.close()

def compound_search(term):
  conn = sqlite3.connect(db)
  conn.text_factory = str
  conn.row_factory = sqlite3.Row
  cur = conn.cursor()
  cur2 = conn.cursor()

  limit = 5

  term = term.lower()

  cur.execute('CREATE TABLE IF NOT EXISTS compound_alias (term TEXT, compound_id TEXT)')
  cur.execute('CREATE TABLE IF NOT EXISTS compound (id TEXT, names TEXT)')
  cur.execute("SELECT * FROM compound_alias WHERE term = ? LIMIT ?", (term,limit))
  compounds = []
  for r in cur:
    comp = {'id': r['compound_id'], 'names': []}
    cur2.execute("SELECT * FROM compound WHERE id = ?", (r['compound_id'],))
    for c in cur2:
      comp['names'] = c['names'].split('; ')
    compounds.append(comp)
  if len(compounds) == 0:
    q = {'dbkey': 'compound', 'mode': 'bfind', 'max_hit': str(limit), 'keywords': term}
    url = "http://www.genome.jp/dbget-bin/www_bfind_sub?"+urllib.urlencode(q)
    req = urllib2.Request(url=url)
    f = urllib2.urlopen(req)
    b = f.read()

    # The output looks like this
    # <div style="width:600px">
    #   <a href="/dbget-bin/www_bget?cpd:C00029">C00029</a>&nbsp;&nbsp;&nbsp;&nbsp;
    #   <span class="kegdraw"><a href="http://www.genome.jp/dbget-bin/KegDraw.jnlp?--find+C00029"> KegDraw </a></span>&nbsp;&nbsp;
    #   <span class="jmol"><a href="http://www.genome.jp/dbget-bin/Jmol.jnlp?-f+m+compound+C00029"> Jmol </a></span><br>
    #   <div style="margin-left:2em"> UDP-glucose; UDPglucose; UDP-D-glucose; Uridine diphosphate glucose; UDP-alpha-D-glucose</div>
    # </div>

    # Find the compound and the description
    poscpdurl = b.find("/dbget-bin/www_bget?cpd:")
    while poscpdurl != -1:
      start = b.find(">", poscpdurl)
      end = b.find("<", start)
      if start != -1 and end != -1:
        compound_id = b[start+1:end]

      start = b.find("<div style=\"margin-left:2em\">", poscpdurl)
      end = b.find("<", start+30)
      if start != -1 and end != -1:
        names = b[start+30:end]
      else:
        names = ''
      poscpdurl = b.find("/dbget-bin/www_bget?cpd:", poscpdurl+10)
      cur.execute('INSERT INTO compound_alias (term, compound_id) VALUES (?, ?)', (term, compound_id))
      cur.execute('SELECT * from compound WHERE id = ?', [compound_id])
      if not cur.fetchone():
        cur.execute('INSERT INTO compound (id, names) VALUES (?, ?)', (compound_id, names))
      compounds.append({'id':compound_id, 'names':names.split('; ')})

  conn.commit()
  cur.close()
  cur2.close()
  conn.close()
  return compounds

def pathway_search(term):
  term = term.lower()

  compounds = compound_search(term)
  pathways = []
  for c in compounds:
    pathways.extend(pathway_search_single(c['id']))
  return pathways

def pathway_search_single(compound_id):
  conn = sqlite3.connect(db)
  conn.text_factory = str
  conn.row_factory = sqlite3.Row
  cur = conn.cursor()
  cur2 = conn.cursor()

  cur.execute('CREATE TABLE IF NOT EXISTS compound_pathway (compound_id TEXT, pathway_id TEXT)')
  cur.execute('CREATE TABLE IF NOT EXISTS pathway (id TEXT, name TEXT)')
  cur.execute("SELECT * FROM compound_pathway WHERE compound_id = ?", (compound_id,))
  pathways = []
  for r in cur:
    cur2.execute('SELECT * FROM pathway WHERE id = ?', (r['pathway_id'],))
    name = ''
    for p in cur2:
      name = p['name']
    pathways.append({'id':r['pathway_id'], 'name':name})
  if len(pathways) == 0:
    url = "http://www.genome.jp/dbget-bin/www_bget?cpd:"+compound_id
    req = urllib2.Request(url=url)
    f = urllib2.urlopen(req)
    b = f.read()

    # <tr>
    #   <td align="left" valign="top" style="width:5em">
    #     <nobr><a href="/kegg-bin/show_pathway?ko00040+C00029">ko00040</a>&nbsp;&nbsp;</nobr>
    #   </td>
    #   <td align="left">Pentose and glucuronate interconversions</td>
    # </tr>
    start = b.find("/kegg-bin/show_pathway?")
    while start != -1:
      pathway = {}

      # Entry
      s = b.find(compound_id, start)
      e = b.find("<", s)
      pathway['id'] = b[s+len(compound_id)+2:e]

      # Name
      s = b.find("<td align=\"left\">", s)
      e = b.find("</td>", s)
      pathway['name'] = b[s+len("<td align=\"left\">"):e]
      pathways.append(pathway)
      cur.execute('INSERT INTO compound_pathway (compound_id, pathway_id) VALUES (?, ?)', (compound_id, pathway['id']))
      cur.execute('INSERT INTO pathway (id, name) VALUES (?, ?)', (pathway['id'], pathway['name']))
      start = b.find("/kegg-bin/show_pathway?",start+5)

  conn.commit()
  cur.close()
  cur2.close()
  conn.close()
  return pathways
