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
  return system_settings_form($form);
}

/**
 * Form submit handler for adding default twitter account and removing any whitespace and @ symbol.
 */
function twitter_account_add_form_submit($form, &$form_state) {
  if ($form_state['values']['screen_name']) {
    global $user;
    $name = $form_state['values']['screen_name'];
    // Removing the @ symbol if any and any white space,
    $name =  preg_replace('@', '', trim($name));
    db_merge('twitter_autopost_users')
      ->fields(array(
        'screen_name' => $name,
        'uid' => $user->uid,
        'is_default' => $form_state['values']['is_default'],
    ))
    ->execute();
  }
}
