<?php

include('selector.inc');

$page = 'https://www.drupal.org/node/2670636';

$source = @file_get_contents($page);

$dom = new SelectorDom($source);

$bullets = $dom->select('.content ul:first-child li');
$dataPoints = array();


foreach ($bullets as $bullet) {
  $data = $bullet['text'];
  parseBulletText($dataPoints, $data);
}

var_dump($dataPoints);

function parseBulletText(&$dataPoints, $text) {
  static $dataPointParsers;
  if (!$dataPointParsers) {
    $dataPointParsers = constructChain();
  }

  foreach ($dataPointParsers as $parser) {
    $parsed_data = $parser->getDataPoint($text);
    if (!is_null($parsed_data)) {
      $dataPoints[$parser->getDataPointName()] = $parsed_data;
      break ;
    }
  }
  
}

function constructChain() {
  return array(
    new ParseAdvisoryId(),
  );
}

abstract class DataPointParser {

  protected $dataPointName;

  public function __construct($dataPointName) {
    $this->dataPointName = $dataPointName;
  }

  public function getDataPointName() { return $this->dataPointName; }

  abstract public function getDataPoint($text);
}

class ParseAdvisoryId extends DataPointParser {
  public function __construct() { parent::__construct('advisoryId'); }

  public function getDataPoint($text) {
    $matches = array();
    if (preg_match('~^Advisory ID: (.*)$~', $text, $matches)) {
      return $matches[1];
    }
    else {
      return null;
    }
  }
}

