<?php

/**
 * @file
 * Drush integration for Superfish.
 */

/**
 * The Superfish plugin URI.
 */
define('SUPERFISH_DOWNLOAD_URI', 'https://github.com/mehrpadin/Superfish-for-Drupal/zipball/master');

/**
 * Implements hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * See `drush topic docs-commands` for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function superfish_drush_command() {
  $items = array();

  // the key in the $items array is the name of the command.
  $items['superfish-plugin'] = array(
    'callback' => 'drush_superfish_plugin',
    'description' => dt("Downloads the Superfish plugin."),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap.
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Superfish plugin. If omitted Drush will use the default location.'),
    ),
    'aliases' => array('superfishplugin'),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 *
 * This function is called whenever a drush user calls
 * 'drush help <name-of-your-command>'
 *
 * @param
 *   A string with the help section (prepend with 'drush:')
 *
 * @return
 *   A string with the help text for your command.
 */
function superfish_drush_help($section) {
  switch ($section) {
    case 'drush:superfish-plugin':
      return dt("Downloads the Superfish plugin, default location is sites/all/libraries.");
  }
}

/**
 * Command to download the Superfish plugin.
 */
function drush_superfish_plugin() {
  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  $args = func_get_args();
  if (!empty($args[0])) {
    $path = $args[0];
  }
  else {
    $path = 'sites/all/libraries';
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  $filename = basename(SUPERFISH_DOWNLOAD_URI) . '.zip';
  $dirname = 'superfish';

  // Remove any existing Superfish plugin directory
  if (is_dir($dirname)) {
    drush_log(dt('A existing Superfish plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }
  // Remove any existing Superfish plugin zip archive
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive
  if (!drush_shell_exec('wget ' . SUPERFISH_DOWNLOAD_URI . ' -O ' . $filename)) {
    drush_shell_exec('curl -O ' . SUPERFISH_DOWNLOAD_URI);
  }

  if (is_file($filename)) {
    // Decompress the zip archive
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Rename directory
    drush_shell_exec('mv mehrpadin-Superfish-for-Drupal-* ' . $dirname);
    // Remove the zip archive
    drush_op('unlink', $filename);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('Superfish plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the Superfish plugin to @path', array('@path' => $path)), 'error');
  }
}