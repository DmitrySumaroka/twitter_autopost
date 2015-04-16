<?php

/**
 * Gives a form for user to enter the default autopost account.
 */
function twitter_autopost_admin_node_settings_form($form, $form_state, $node_names = array()) {
  // Needs to be refactored... How?? Idk yet.
  $node_types = node_type_get_types();
  $node_names = array();
  $default_values_type = twitter_autopost_get_node_type_saved();
  $saved_node_values = array();
  $default_values = array();
  foreach ($default_values_type as $value) {
    array_push($saved_node_values, $value->node_type);
  }
  $counter = 0;
  foreach ($node_types as $type) {
    if (in_array($type->type, $saved_node_values)) {
      array_push($default_values, $counter);
    }
    else {
      array_push($default_values, NULL);
    }
    array_push($node_names, $type->name);
    $counter++;
  }
  $form['settings'] = array(
    '#markup' => l('Back to Settings', '/admin/config/services/twitter-autopost'),
  );
  $form['node_type'] = array(
    '#title' => t('For which content types should this be available?'),
    '#type' => 'checkboxes',
    '#options' => $node_names,
    '#default_value' => $default_values,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

/**
 * Form submit handler for adding default twitter account and removing any whitespace and @ symbol.
 */
function twitter_autopost_admin_node_settings_form_submit($form, &$form_state) {
  $node_types = node_type_get_types();
  $node_names = array();
  foreach ($node_types as $type) {
    array_push($node_names, $type->type);
  }
  $submitted_types = $form_state['values']['node_type'];
  foreach ($submitted_types as $key => $value) {
    // If they are not checked they will be integers...
    if (is_string($value)) {
      twitter_autopost_add_node_screen_name($node_names[$key]);
    }
    else {
      twitter_autopost_remove_node_screen_name($node_names[$key]);
    }
  }
}

/**
 * Gives a form for user to enter the default autopost account.
 */
function twitter_autopost_admin_settings_form($form, $form_state) {
  $form['node-settings'] = array(
    '#markup' => l('Node Settings','/admin/config/services/twitter-autopost-node-settings'),
  );
  $form['account'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twitter Account To Use'),
    '#suffix' => t('For example "@twitterAccount"'),
  );
  $form['is_default'] = array(
    '#title' => t('Should this ID be the default Twitter name?'),
    '#type' => 'checkbox',
    '#default_value' => 0,
  );
  $form['submit_account_name'] = array(
    '#type' => 'submit',
    '#value' => t('Add Account'),
  );
  return $form;
}

/**
 * Form submit handler for adding default twitter account and removing any whitespace and @ symbol.
 */
function twitter_autopost_admin_settings_form_submit($form, &$form_state) {
  if ($form_state['values']['account']) {
    global $user;
    $name = $form_state['values']['account'];
    // Removing the @ symbol if any and any white space,
    if (preg_match('/\s/', $name)) {
      drupal_set_message("Please remove any spaces in the name", 'error');
      return;
    }
    if (substr($name, 0, 1) == "@") {
      $name = substr($name, 1, strlen($name));
    }
    $result = db_query("SELECT screen_name FROM twitter_autopost_users WHERE screen_name=:name", array(':name' => $name));
    if ($result->fetchObject()) {
      drupal_set_message("This screen name already exists", 'error');
      return;
    }
    twitter_autopost_save_screen_name($name, $user->uid, $form_state['values']['is_default'], TRUE);
  }
}

/**
 * Gives a form for user to enter the default autopost account.
 */
function twitter_autopost_admin_account_form($form, $form_state) {
  $accounts = array();
  $screen_names = array();
  $default_account = '';
  $form['signed_in_accounts_header'] = array(
  '#markup' => '<h2>Current Accounts</h2>' ,
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'twitter_autopost') . '/twitter_autopost.css',
  );
  $result = db_query('SELECT * FROM twitter_autopost_users');
  foreach ($result as $account) {
    array_push($accounts, $account);
  }
  foreach ($accounts as $value) {
    if($value->is_default) {
      array_unshift($screen_names, $value->screen_name);
      $default_account = $value->screen_name;
    }
    else {
      array_push($screen_names, $value->screen_name);
    }
  }
  if (!$screen_names) {
    $form['account-empty'] = array(
      '#markup' => '<h2>No Accounts Added</h2>',
    );
  }
  else {
    if ($default_account) {
      $form['signed_in_accounts_header'] = array(
        '#markup' => '<h3>Default Account: ' . $default_account . '</h2>' ,
      );
    }
    $form['signed_in_accounts'] = array(
      '#type' => 'select',
      '#options' => $screen_names,
    );
    $form['delete_account'] = array(
      '#type' => 'checkbox',
      '#title' => 'Delete selected account',
    );
    $form['make_account_default'] = array(
      '#type' => 'checkbox',
      '#title' => 'Make selected account default',
    );
    $form['save_account_details'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }
  return $form;
}

/**
 * Form submit handler for adding default twitter account and removing any whitespace and @ symbol.
 */
function twitter_autopost_admin_account_form_submit($form, &$form_state) {
  $screen_name_ids = array();
  $accounts = array();
  $result = db_query('SELECT * FROM twitter_autopost_users');
  foreach ($result as $account) {
    array_push($accounts, $account);
  }
  foreach ($accounts as $value) {
    if($value->is_default) {
      array_unshift($screen_name_ids, $value->screen_name_id);
    }
    else {
      array_push($screen_name_ids, $value->screen_name_id);
    }
  }
  $selected_screen_name = $screen_name_ids[$form_state['values']['signed_in_accounts']];
  if ($form_state['values']['delete_account']) {
    twitter_autopost_delete_screen_name($selected_screen_name, TRUE);
  }
  else if ($form_state['values']['make_account_default']) {
    twitter_autopost_set_default_screen_name($selected_screen_name);
  }
}

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
