<?php

/**
 * Insert a new record of a screen name.
 */
function twitter_autopost_save_screen_name($name, $uid, $is_default, $set_message = FALSE) {
  // Check for any defaults here and set them to false if is_default is set to true
  if ($is_default) {
    twitter_autopost_remove_default_screen_name();
  }
  db_insert('twitter_autopost_users')
    ->fields(array(
      'screen_name' => $name,
      'uid' => $uid,
      'is_default' => $is_default,
  ))
  ->execute();
  if ($set_message) {
    drupal_set_message("Account Saved", 'status');
  }
}

/**
 * Delete a record of a screen name.
 */
function twitter_autopost_delete_screen_name($screen_name_id, $set_message = FALSE) {
  db_delete('twitter_autopost_users')
  ->condition('screen_name_id', $screen_name_id)
  ->execute();
  if ($set_message) {
    drupal_set_message("Account Deleted", 'status');
  }
}

/**
 * Set a record as default.
 */
function twitter_autopost_set_default_screen_name($screen_name_id, $set_message = FALSE) {
  twitter_autopost_remove_default_screen_name();
  db_update("twitter_autopost_users")
    ->fields(array(
      'is_default' => 1,
    ))
    ->condition('screen_name_id', $screen_name_id)
    ->execute();
  if ($set_message) {
    drupal_set_message("Set a new default account", 'status');
  }
}

/**
 * Set a remove default from a record.
 */
function twitter_autopost_remove_default_screen_name() {
  db_update("twitter_autopost_users")
    ->fields(array(
      'is_default' => 0,
    ))
    ->condition('is_default', 1)
    ->execute();
}

/**
 * Add a default node for twitter_autopost to use.
 */
function twitter_autopost_add_node_screen_name($node_type) {
  $result = db_query('SELECT node_type_id FROM twitter_autopost_node_types WHERE node_type=:type', array(':type' => $node_type));
  $result_empty = TRUE;
  $result_node_type_id = '';
  foreach ($result as $value) {
    if ($value) {
      $result_empty = FALSE;
    }
  }
  if ($result_empty) {
    db_insert('twitter_autopost_node_types')
      ->fields(array(
        'node_type' => $node_type,
        'use_autopost' => 1,
    ))
    ->execute();
  }
}

/**
 * Check if default node is set and remove for twitter_autopost to use.
 */
function twitter_autopost_remove_node_screen_name($node_type) {
  db_delete('twitter_autopost_node_types')
    ->condition('node_type', $node_type)
    ->execute();
}

/**
 * Get node types that are saved.
 */
function twitter_autopost_get_node_type_saved() {
  $result = db_query('SELECT * FROM twitter_autopost_node_types');
  $node_types = array();
  foreach ($result as $value) {
    array_push($node_types, $value);
  }
  return $node_types;
}