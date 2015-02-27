<?php

/**
 * Gives a form for user to enter the default autopost account
 */
function twitter_account_add_form($form, $form_state) {
  $form['account'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twitter Account To Use'),
    '#suffix' => t('For example "@twitterAccount"'),
  );

  $form['sumbmit_account_name'] = array(
    '#type' => 'submit',
    '#value' => t('Make default account'),
  );
}

