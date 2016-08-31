<?php

class ParserChain {
  private $dataPointParsers;

  public function __construct() {
    $this->dataPointParsers = array(
      new ParseAdvisoryId(),
    );
  }

  public function parseBulletText($text) {

    foreach ($this->dataPointParsers as $parser) {
      $parsed_data = $parser->getDataPoint($text);
      if (!is_null($parsed_data)) {
        return array($parser->getDataPointName(), $parsed_data);
      }
    }

    return array(null, null);
  }
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

