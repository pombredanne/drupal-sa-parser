<?php

include_once('selector.inc');
include_once('DrupalSAPageReader.inc');

$spider = new Spider(22);
$data = $spider->getAdvisoryData();
foreach ($data as $url => $advisoryData) {
	print outputAdvisory($url, $advisoryData) . "\n";
}





class Spider {

	private $maxPage;
	private $baseUrl = 'https://www.drupal.org/security/contrib';

	public function __construct($maxPage) {
		$this->maxPage = $maxPage;
	}

	public function getAdvisoryData() {
		$urlsToSpider = array($this->baseUrl);

		for ($i = 1; $i <= $this->maxPage; $i++) {
			$urlsToSpider[] = $this->baseUrl . "?page=$i";
		}

		$advisories = array();
		foreach ($urlsToSpider as $url) {
			$advisories = array_merge($advisories, $this->getAdvisories($url));
		}

		$data = array();
		foreach ($advisories as $advisory) {
			$reader = new DrupalSAPageReader($advisory);
			$data[$advisory] = $reader->parse();
		}

		return $data;
	}

	private function getAdvisories($url) {
		$source = @file_get_contents($url);

		$dom = new SelectorDom($source);

		$links = $dom->select('.content .view-drupalorg-security-announcements-contrib .views-row .node h2 a');

		return array_map(function ($a) { return 'https://www.drupal.org' . $a['attributes']['href']; }, $links);
	}
}

function outputAdvisory($url, $data) {
	$flat = array(
		$data['advisoryId'],
		$data['date']['year'] . '-' . $data['date']['month'] . '-' . $data['date']['day'],
		$data['risk'],
		$data['project']['id'],
		is_array($data['version']) ? join(';', $data['version']) : $data['version'],
		$url,
	);

	return join(",", $flat);
}