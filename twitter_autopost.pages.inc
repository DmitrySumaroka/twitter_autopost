<?php

/**
 * Gives a form for user to enter the default autopost account.
 */
function twitter_autopost_admin_node_settings_form($form, $form_state) {
  _node_list_row();
  return $form;
}

/**
 * Form submit handler for adding default twitter account and removing any whitespace and @ symbol.
 */
function twitter_autopost_admin_node_settings_form_submit($form, &$form_state) {

}


/**
 * Gives a form for user to enter the default autopost account.
 */
function twitter_autopost_admin_settings_form($form, $form_state) {
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
    '#description' => t("Choose this to make it a site-wide default"),
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
  $form['signed_in_accounts_header'] = array(
  '#markup' => '<h2>Current Accounts</h2>' ,
  );
  $form['#attached']['css'] = array(
    drupal_get_path('module', 'twitter_autopost') . '/twitter_autopost.css',
  );
  $result = db_query('SELECT * FROM twitter_autopost_users');
  foreach ($result as $account) {
    $form['signed_in_accounts'][] = _account_list_row($account);
  }
  if (!$form['signed_in_accounts']) {
    $form['account-empty'] = array(
      '#markup' => '<h2>No Accounts Added</h2>',
    );
  }
  $form['add_account'] = array(
    '#markup' => '<div class="twitter-autopost-add-link">' . l('Add Account', '/admin/config/services/twitter-autopost-add') . '</div>',
  );
  return $form;
}

/**
 * Form submit handler for adding default twitter account and removing any whitespace and @ symbol.
 */
function twitter_autopost_admin_account_form_submit($form, &$form_state) {

  dsm($form_state);
  if ($form_state['values']['delete']) {
    twitter_autopost_delete_screen_name($screen_name_id, TRUE);
  }
  if ($form_state['values']['delete']) {
    twitter_autopost_set_default_screen_name($screen_name_id);
  }
}

/**
 * Returns the form fields to manage a Twitter account.
 */
function _account_list_row($account) {
  $form['#account'] = $account;
  $form['accounts-signed-in'] = array(
    '#markup' => '<div id="twitter-autopost-signed-in-accounts">',
  );
  $form['screen_name_id'] = array(
    '#type' => 'value',
    '#default_value' => intval($account->screen_name_id),
  );
  $form['screen_name'] = array(
    '#markup' => '<div id="twitter-autopost-screen-name" data-twitter-autopost-name-id=' . $account->screen_name_id . '>' . '@' . $account->screen_name . '</div>',
    '#value' => $account->screen_name,
  );
  $form['default'] = array(
    '#title' => 'Default',
    '#type' => 'checkbox',
    '#default_value' => $account->is_default,
  );
  $form['delete'] = array(
    '#title' => 'Delete',
    '#type' => 'checkbox',
  );
  $form['signed_in_accounts']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['end-form'] = array(
    '#markup' => '</div>',
  );
  return $form;
}

/**
 * Returns the form fields to manage a node types Twitter account.
 */
function _node_list_row() {
  dsm(node_type_get_types());
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
      'is_default' => 0,
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
