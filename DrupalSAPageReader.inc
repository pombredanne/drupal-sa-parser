<?php

include('selector.inc');
include('ParserChain.inc');

class DrupalSAPageReader {
	private $url;
	public function __construct($url) {
		$this->url = $url;
	}

	public function parse() {
		$source = @file_get_contents($this->url);

		$dom = new SelectorDom($source);
		$chain = new ParserChain();

		$bullets = $dom->select('.content ul:first-child li', false);
		$rawData = array();

		for ($i = 0; $i < $bullets->length; $i++) {
			$item = $bullets->item($i);
			$rawData[] = $this->DOMinnerHTML($item);
		}

		$dataPoints = array();

		foreach ($rawData as $innerHtml) {
		  list($key, $value) = $chain->parseBulletText($innerHtml);
		  if ($key) {
		    $dataPoints[$key] = $value;
		  }
		}

		return $dataPoints;	
	}

	// see http://stackoverflow.com/a/2087136 
	private function DOMinnerHTML(DOMNode $element) { 
	    $innerHTML = ""; 
	    $children  = $element->childNodes;

	    foreach ($children as $child) { 
	        $innerHTML .= $element->ownerDocument->saveHTML($child);
	    }

	    return $innerHTML; 
	}
}

function testDrupalSAPageReader() {
	$reader = new DrupalSAPageReader('https://www.drupal.org/node/2670636');
	$dataPoints = $reader->parse();
	var_dump($dataPoints);

	assert($dataPoints['advisoryId'] == 'DRUPAL-SA-CONTRIB-2016-007', 'Advisory ID');
	assert($dataPoints['version'][0] == '7.x', 'Version - first');
	assert($dataPoints['version'][1] == '8.x', 'Version - second');
	assert($dataPoints['risk'] == 'Moderately critical', 'Risk');
	assert($dataPoints['date']['year'] == 2016, 'Date year');
	assert($dataPoints['date']['month'] == 2, 'Date month');
	assert($dataPoints['date']['day'] == 17, 'Date day');
	assert($dataPoints['project']['id'] == 'nodejs', 'Project id');
	assert($dataPoints['project']['name'] == 'Node.js integration', 'Project name');
}

testDrupalSAPageReader();
