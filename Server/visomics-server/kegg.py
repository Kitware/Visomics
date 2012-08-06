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

from SOAPpy import WSDL
import json
import xml.etree.cElementTree as ElementTree
import urllib2
import sys, os.path

cache_nameToID = {}
cache_IDToPathways = {None: []}
cache_IDToTitles = {None: []}
cache_pathwayInfo = {None: []}
cache_pathwayMap = {None: []}

wsdl = 'http://soap.genome.jp/KEGG.wsdl'
serv = None

# --------------------------------------------------------------------------
def IDsToTitles(compoundIDs):
  unknownIDs = [compoundID for compoundID in compoundIDs if compoundID not in cache_IDToTitles]
  if unknownIDs:
    # serv.btit only accepts up to 100 IDs at a time
    results = ""
    chunkMax = 0
    while chunkMax < len(unknownIDs):
      chunkMin = chunkMax;
      chunkMax += min(100, len(compoundIDs)-chunkMax)
      results += serv.btit(" ".join(compoundIDs[chunkMin:chunkMax]))
    resultList = results.rstrip("\n").split("\n")
    for result in resultList:
      splitResult = result.partition(" ")
      titleList = splitResult[2].split("; ")
      cache_IDToTitles[splitResult[0]] = titleList
  return [(compoundID, cache_IDToTitles[compoundID]) for compoundID in compoundIDs]

# --------------------------------------------------------------------------
def IDToTitles(compoundID):
  return IDsToTitles([compoundID])[0][1]

# --------------------------------------------------------------------------
def nameToID(name):
  nameLower = name.lower()
  if nameLower not in cache_nameToID:
    results = serv.search_compounds_by_name(nameLower)
    if len(results) == 0:
      cache_nameToID[nameLower] = None
    elif len(results) == 1:
      cache_nameToID[nameLower] = results[0]
    else:
      found = False
      for compoundID, titles in IDsToTitles(results):
        if nameLower in [title.lower() for title in titles]:
          cache_nameToID[nameLower] = compoundID
          found = True
          break
      # Note: cache_IDToTitles is NOT cleared of the non-matches
      if not found:
        print "No exact match for %s, must manually find" % name
        cache_nameToID[nameLower] = None
  return cache_nameToID[nameLower]

# --------------------------------------------------------------------------
def IDToPathways(compoundID):
  if compoundID not in cache_IDToPathways:
    results = serv.get_pathways_by_compounds([compoundID])
    cache_IDToPathways[compoundID] = [i for i in results]  #convert from a SOAP list to a python list
  return cache_IDToPathways[compoundID]

# --------------------------------------------------------------------------
def pathwayInfo(pathwayID):
  if pathwayID not in cache_pathwayInfo:
    KGML_URL = 'http://www.genome.jp/kegg-bin/download?entry=' + pathwayID.lstrip('path:') + '&format=kgml'
    KGML_tree = ElementTree.parse(urllib2.urlopen(KGML_URL))
    pathwayTitle = KGML_tree.getroot().get('title')
  #  nodes = {}
  #  for entryElem in KGML_tree.getroot().findall('entry'):
  #    if entryElem.get('type') == 'compound':
  #      nodes[entryElem.get('id')] = entryElem.get('name')
    reactions = []
    for reactionElem in KGML_tree.getroot().findall('reaction'):
      #reactions[reactionElem.get('name')]
      for substrateElem in reactionElem.findall('substrate'):
        reactions.append( (substrateElem.get('name'), reactionElem.get('name')) )
      for productElem in reactionElem.findall('product'):
        reactions.append( (reactionElem.get('name'), productElem.get('name')) )
    cache_pathwayInfo[pathwayID] = reactions
  return cache_pathwayInfo[pathwayID]

# --------------------------------------------------------------------------
def pathwayMap(pathwayID):
  if pathwayID not in cache_pathwayMap:
    map_URL = serv.mark_pathway_by_objects(pathwayID, [])
    cache_pathwayMap[pathwayID] = urllib2.urlopen(map_URL).read()
  return cache_pathwayMap[pathwayID]

# --------------------------------------------------------------------------
def handleQuery(queryType, queryList):
  returnList = []
  if queryType == 'compound':
    # Prefetch compound and pathway titles; can do multiple compounds in 1 SOAP query
    titCompoundIDs = [nameToID(value) for name, value in queryList if (name == 'tit' or name == 'all')]
    pathCompoundIDs = [nameToID(value) for name, value in queryList if (name == 'path' or name == 'all')]
    if titCompoundIDs:
      IDsToTitles(titCompoundIDs)
    if pathCompoundIDs:
      allPathways = set()
      for compoundID in pathCompoundIDs:
        allPathways.update(IDToPathways(compoundID))
      IDsToTitles(list(allPathways))
    # Actual fetches for return; all info should be cached
    for name, value in queryList:
      compoundID = nameToID(value)
      returnItem = {
        'compound_name': value,
        'compound_id': compoundID }
      if (name == 'tit' or name == 'all'):
        returnItem['compound_titles'] = IDToTitles(compoundID)
      if (name == 'path' or name == 'all'):
        pathways = IDToPathways(compoundID)
        returnItem['compound_pathways'] = [(pathway, IDToTitles(pathway)[0]) for pathway in pathways]
      returnList.append(returnItem)
  elif queryType == 'graph':
    for name, value in queryList: # assume name == 'path'
      graph = pathwayInfo(value)
      returnList.append({
          'pathway_id': value,
          'pathway_graph': graph
          })
  elif queryType == 'map':
    for name, value in queryList: # assume name == 'path'
      pathMap = pathwayMap(value)
      return True, pathMap, 'image/png'
  else: # invalid queryType
    return False, "", ""
  return True, json.dumps(returnList), 'application/json'

# --------------------------------------------------------------------------
def loadfileNameToID(filename):
  rawDict = json.load(open(os.path.join(sys.path[0], filename)))
  # convert all from unicode to python strings
  for key in rawDict:
    if rawDict[key] == None:
      cache_nameToID[key.encode('utf-8')] = None
    else:
      cache_nameToID[key.encode('utf-8')] = rawDict[key].encode('utf-8')

# --------------------------------------------------------------------------
def init(prefetch = True):
  global serv
  serv = WSDL.Proxy(wsdl)
  if prefetch:
    print "KEGG Prefetching..."
    loadfileNameToID('prefetch_UNC.json')
    validIDs = [ID for ID in cache_nameToID.values() if ID]
    IDsToTitles(validIDs)
    allPathways = set()
    for ID in validIDs:
      allPathways.update(IDToPathways(ID))
    IDsToTitles(list(allPathways))
    print "  done"
