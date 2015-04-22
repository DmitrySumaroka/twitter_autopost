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
      twitter_autopost_add_node_screen_name($node_names[$key], TRUE);
    }
    else {
      twitter_autopost_remove_node_screen_name($node_names[$key], TRUE);
    }
  }
}

/**
 * Gives a form for user to enter the default autopost account.
 */
function twitter_autopost_admin_account_form($form, $form_state) {
  $accounts = array();
  $screen_names = array();
  $default_account = '';
  $form['node-settings'] = array(
    '#markup' => l('Node Settings','/admin/config/services/twitter-autopost-node-settings'),
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
    $form['signed_in_accounts_header'] = array(
      '#markup' => '<h2>Current Accounts</h2>' ,
    );
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
 * Form submit handler for making twitter account default or deleting a screen name.
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
