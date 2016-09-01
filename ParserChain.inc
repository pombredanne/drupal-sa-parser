<?php

class ParserChain {
  private $dataPointParsers;

  public function __construct() {
    $this->dataPointParsers = array(
      new ParseAdvisoryId(),
      new ParseVersion(),
      new ParseRisk(),
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

class ParseVersion extends DataPointParser {
  public function __construct() { parent::__construct('version'); }

  public function getDataPoint($text) {
    $matches = array();
    if (preg_match('~^Version: (.*)$~', $text, $matches)) {
      $csvVersions = $matches[1];

      if ($csvVersions) {
        $versions = explode(',', $csvVersions);
      }

      if (!is_array($versions)) {
        return null;
      }

      $versions = array_filter(array_map('trim', $versions));
      return $versions;
    }
    else {
      return null;
    }
  }
}

class ParseRisk extends DataPointParser {

  private $levels = array (
    'Highly critical',
    'Moderately critical',
    'Critical',
    'Less critical',
    'Not critical',
  );

  public function __construct() { parent::__construct('risk'); }

  public function getDataPoint($text) {
    $matches = array();
    if (!preg_match('~^Security risk: [^\(]*\(([^\)]*)\).*$~', $text, $matches)) {
      return null;
    }

    $payload = $matches[1];
    foreach ($this->levels as $level) {
      if (stripos($payload, $level) !== false) {
        return $level;
      }
    }

    return null;
  }
}
