<?php

class ParserChain {
  private $dataPointParsers;

  public function __construct() {
    $this->dataPointParsers = array(
      new ParseAdvisoryId(),
      new ParseVersion(),
      new ParseRisk(),
      new ParseDate(),
      new ParseProject(),
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

class ParseDate extends DataPointParser {
  public function __construct() { parent::__construct('date'); }

  private $replacements = array(
    'January' => 'Jan',
    'February' => 'Feb',
    'March' => 'Mar',
    'April' => 'Apr',
    'May' => 'May',
    'June' => 'Jun',
    'July' => 'Jul',
    'August' => 'Aug',
    'September' => 'Sep',
    'October' => 'Oct',
    'November' => 'Nov',
    'December' => 'Dec',
  );

  public function getDataPoint($text) {
    $matches = array();
    if (!preg_match('~^Date: (.*)$~', $text, $matches)) {
      return null;
    }

    $dateToParse = $matches[1];
    $dateToParse =
      str_replace(array_keys($this->replacements), array_values($this->replacements), $dateToParse);

    $date = date_parse($dateToParse);

    return array(
      'year' => $date['year'],
      'month' => $date['month'],
      'day' => $date['day'],
    );
  }
}


class ParseProject extends DataPointParser {
  public function __construct() { parent::__construct('project'); }

  public function getDataPoint($text) {
    $matches = array();
    if (!preg_match('~<a[^>]*drupal.org/project/([a-zA-Z0-9_\-]+)[^>]*>([^<]*)</a>~', $text, $matches)) {
      return null;
    }

    $projectId = $matches[1];
    $projectName = $matches[2];

    return array(
      'id' => $projectId,
      'name' => $projectName,
    );
  }
}
