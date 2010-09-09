<?php
// $Id$

/**
 * @file
 *   Workbench Access admin file.
 */

/**
 * Test callback for admin.
 */
function workbench_access_settings_form(&$form, &$form_state) {
  $info = module_invoke_all('workbench_access_info');
  $options = array('test' => 'Test');
  foreach ($info as $key => $value) {
    $options[$key] = $value['name'] . '<br /><em>' . $value['description'] . '</em>';
  }
  $active = variable_get('workbench_access', 'workbench_access');
  $form = array();
  $form['workbench_access'] = array(
    '#type' => 'radios',
    '#title' => t('Active access scheme'),
    '#options' => $options,
    '#default_value' => $active,
    '#description' => t('Select the access scheme to use for the site.'),
    '#required' => TRUE,
  );
  // Get the configuration options for the active schemes.
  foreach ($info as $key => $value) {
    if (isset($info[$key]['configuration']) && function_exists($info[$key]['configuration'])) {
      $func = $info[$key]['configuration'];
      $func($form, $form_state);
      foreach (array('validate', 'submit') as $action) {
        if (function_exists($func . '_' . $action)) {
          $form['#' . $action][] = $func . '_' . $action;
        }
      }
    }
  }
  /*
  $options = array(
    t('Always assert response'),
    t('Always assert on TRUE'),
    t('Always assert on FALSE'),
  );
  $form['workbench_access_behavior'] = array(
    '#type' => 'radios',
    '#title' => t('Node access integration'),
    '#options' => $options,
    '#default_value' => variable_get('workbench_access_behavior', 0),
    '#description' => t('Define how to intract with node access declarations.'),
  );*/
  return system_settings_form($form);
}

/**
 * Administer section definitions.
 */
function workbench_access_section_form(&$form, &$form_state) {
  $scheme = variable_get('workbench_access', 'workbench_access');
  $func = $scheme . '_workbench_access_info';
  $info = $func();
  $data = $info[$scheme];
  $active = workbench_access_get_ids_by_scheme($data);
  $tree = workbench_access_tree($data);
  workbench_build_tree($tree);
  $form = array();
  $form['access_scheme'] = array(
    '#type' => 'value',
    '#value' => $data,
  );
  $form['sections']['#tree'] = TRUE;
  $parent = 0;
  $used = array();
  foreach ($tree as $section) {
    if (in_array($section['access_id'], $used)) {
      continue;
    }
    $states = array();
    $conditions = array();
    if ($section['depth'] > 0) {
      $conditions[] = array(':input[name=sections[' . $section['access_id'] . ']]' => array('checked' => TRUE));
      $conditions[] = array(':input[name=sections[' . $section['parent'] . ']]' => array('checked' => TRUE));
      if (!empty($section['children'])) {
        foreach ($section['children'] as $child_id) {
          $conditions[] = array(':input[name=sections[' . $child_id . ']]' => array('checked' => TRUE));
        }
      }
    }
    if (!empty($conditions)) {
      $states = array(
        'visible' => $conditions,
      );
    }
    $form['sections'][$section['access_id']] = array(
      '#type' => 'checkbox',
      '#title' => (!empty($section['children']) ? '+ ' : '') . str_repeat('-', $section['depth']) . $section['name'] . ' id: ' . $section['access_id'] . ' p:' . $section['parent'] . ' d:' . $section['depth'],
      '#default_value' => isset($active[$section['access_id']]),
      '#states' => $states,
    );
    $used[] = $section['access_id'];
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save sections'),
  );
  return $form;
}

function workbench_build_tree(&$tree, $sections = NULL, $depth = -1) {
  static $max_depth;
  if (!isset($max_depth)) {
    $max_depth = 0;
  }
  if ($depth > $max_depth) {
    return;
  }
  $depth++;
  foreach ($tree as $id => $item) {
    if ($item['depth'] > $max_depth) {
      $max_depth = $item['depth'];
    }
    if ($depth == 0 && $item['parent'] > 0 && isset($tree[$item['parent']])) {
      $tree[$item['parent']]['children'][] = $id;
    }
    elseif ($item['depth'] > 0 && !empty($item['children']) && isset($tree[$item['parent']]['children'])) {
      $tree[$item['parent']]['children'] = array_unique(array_merge($tree[$item['parent']]['children'], $item['children']));
    }
  }
  workbench_build_tree($tree, $sections, $depth);
}

function workbench_access_section_form_submit(&$form, &$form_state) {
  $access_scheme = $form_state['values']['access_scheme'];
  $sections = array_keys(array_filter($form_state['values']['sections']));
  workbench_access_rebuild_scheme($access_scheme, $sections);
  if (empty($sections)) {
    drupal_set_message(t('There are now no editorial sections for your site.'), 'warning');
  }
  else {
    drupal_set_message(t('Editorial sections saved.'));
  }
}

function workbench_access_user_form(&$form, &$form_state, $account) {
  $scheme = variable_get('workbench_access', 'workbench_access');
  $func = $scheme . '_workbench_access_info';
  $info = $func();
  $data = $info[$scheme];
  $active = workbench_access_get_ids_by_scheme($data, TRUE);
  $tree = workbench_access_tree($data);
  $items = workbench_build_tree($tree);
  $form = array();
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $account->uid,
  );
  $form['access_scheme'] = array(
    '#type' => 'value',
    '#value' => $data,
  );
  $sections = $account->workbench_access;
  $form['user_sections'] = array(
    '#type' => 'value',
    '#value' => array_keys($sections),
  );
  $form['sections']['#tree'] = TRUE;
  $start = array_shift($items);
  foreach ($start as $depth => $parent) {
    foreach ($parent as $section) {
      $form['sections'][$section['access_id']] = array(
        '#type' => 'checkbox',
        '#title' => $section['name'] . ' id: ' . $section['access_id'] . ' p:' . $section['parent'] . ' d:' . $section['depth'],
        '#default_value' => isset($sections[$section['access_id']]),
        '#disabled' => !in_array($section['access_id'], $active),
      );
    }
  }
  foreach ($items as $depth => $parent) {
    foreach ($parent as $section) {
      foreach ($section as $parent_id => $item) {
        $form['sections'][$item['access_id']] = array(
          '#type' => 'checkbox',
          '#title' => $item['name'] . ' id: ' . $item['access_id'] . ' p:' . $item['parent'] . ' d:' . $item['depth'],
          '#default_value' => isset($sections[$item['access_id']]),
          '#disabled' => !in_array($item['access_id'], $active),
        );
      }
    }
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save sections'),
  );
  return $form;
}

function workbench_access_user_form_submit($form, &$form_state) {
  $access_scheme = $form_state['values']['access_scheme'];
  $user_sections = $form_state['values']['user_sections'];
  $sections = array_keys(array_filter($form_state['values']['sections']));
  if (empty($sections)) {
    drupal_set_message(t('There are now no editorial sections for this user.'), 'warning');
  }
  $uid = $form_state['values']['uid'];
  workbench_access_rebuild_user($uid, $access_scheme, $sections, $user_sections);
  if (!empty($sections)) {
    drupal_set_message(t('User permissions updated.'));
  }
}