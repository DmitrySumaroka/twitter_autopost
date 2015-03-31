<?php

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
    '#value' => t('Make default account'),
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
    if(substr($name, 0, 1) == "@") {
      $name = substr($name, 1, strlen($name));
    }
    twitter_autopost_save_screen_name($name, $user->uid, $form_state['values']['is_default']);
  }
}

/**
 * Insert a new record of a screen name.
 */
function twitter_autopost_save_screen_name($name, $uid, $is_default) {
  // Check for any defaults here and set them to false if is_default is set to true
  if ($is_default) {
    db_update("twitter_autopost_users")
    ->fields(array(
      'is_default' => 0,
    ))
    ->condition('is_default', 1)
    ->execute();
  }
  db_insert('twitter_autopost_users')
    ->fields(array(
      'screen_name' => $name,
      'uid' => $uid,
      'is_default' => $is_default,
  ))
  ->execute();
}

/**
 * Delete a record of a screen name.
 */
function twitter_autopost_delete_screen_name($name, $uid, $is_default) {

}
