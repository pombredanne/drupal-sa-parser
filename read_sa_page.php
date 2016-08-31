<?php

include('selector.inc');
include('ParserChain.inc');

$page = 'https://www.drupal.org/node/2670636';

$source = @file_get_contents($page);

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

var_dump($dataPoints);
