<?php

include('selector.inc');
include('ParserChain.inc');

function parseSecurityAdvisory($url) {
	$source = @file_get_contents($url);

	$dom = new SelectorDom($source);
	$chain = new ParserChain();

	$bullets = $dom->select('.content ul:first-child li');
	$dataPoints = array();



	foreach ($bullets as $bullet) {
	  $data = $bullet['text'];
	  list($key, $value) = $chain->parseBulletText($data);
	  if ($key) {
	    $dataPoints[$key] = $value;
	  }
	}

	return $dataPoints;	
}

function testParse() {
	$dataPoints = parseSecurityAdvisory('https://www.drupal.org/node/2670636');

	assert($dataPoints['advisoryId'] == 'DRUPAL-SA-CONTRIB-2016-007', 'Advisory ID');
	assert($dataPoints['version'][0] == '7.x', 'Version - first');
	assert($dataPoints['version'][1] == '8.x', 'Version - second');
	assert($dataPoints['risk'] == 'Moderately critical', 'Risk');
	assert($dataPoints['date']['year'] == 2016, 'Date year');
	assert($dataPoints['date']['month'] == 2, 'Date month');
	assert($dataPoints['date']['day'] == 17, 'Date day');

	return $dataPoints;
}

if ($data = testParse()) {
	print "Hooray!\n";
	var_dump($data);
}
else {
	print "Boo\n";
}