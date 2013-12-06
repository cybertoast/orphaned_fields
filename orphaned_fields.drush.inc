<?php

/**
 * @file
 * Drush command to find all orphaned fields
 *
 * @see http://drupal.org/node/649298
 */

/**
 * Implements hook_drush_command().
 */
function orphaned_fields_drush_command() {

  $items['orphaned-fields'] = array(
    'description' => 'Find all orphaned fields',
    'drupal dependencies' => array('features'),
    'options' => array(
      'show_all' => 'Show all (both MISSING and EXPORTED) fields and their status'
    ),
    'aliases' => array('orf'),
    'callback' => 'drush_orphaned_fields',
  );

  return $items;
}

/**
 * Drush command callback;
 */
function drush_orphaned_fields() {
  module_load_include('inc', 'features', 'features.export');

  $show_all = drush_get_option("show_all");
  
  // Gather up all the fields exported as features
  $commited_fields = array();
  // We can either get feature-export information from:
  //    feature_get_default('field') or feature_get_default('field_base')
  
  // The display-suite stuff may do things to the base field list
  $field_info = features_get_default('field_base', 'ike_features');
  if (empty($field_info)) {
      $field_info = features_get_default('field', 'ike_features');
  }
  foreach ($field_info as $id => $field) {
      if (isset($field['field_config'])) {
        // the raw 'field' will contain a 'field_config' object
        if (!in_array($field['field_config']['field_name'], $commited_fields)) {
          $commited_fields[] = $field['field_config']['field_name'];
        } 
      } elseif (isset($field['field_name'])) {
        // The 'field_base' provides the field-name directly
        if (!in_array($field['field_name'], $commited_fields)) {
          $commited_fields[] = $field['field_name'];
        }           
      }
  }

  drush_print("The following fields are in the database, but are NOT in the exported features\n\n");
  $rows[] = array('Status', 'Field name', 'Bundle');
  $rows[] = array('------', '-------------', '----------');
  foreach (field_info_fields() as $field_name => $field) {
    $bundle_strs = array();
    if (!in_array($field_name, $commited_fields)) {
      // drush_print("$field_name is NOT exported");
      foreach ($field['bundles'] as $entity_type => $bundles) {
        $bundle_strs[] = $entity_type . ' (' . implode(',', $bundles) . ')';
      }
      $rows[]= array( 'MISSING', $field_name, implode(' ', $bundle_strs) );
    } else {
        if (isset($show_all) && ! empty($show_all)) {
          $rows[]= array( 'EXPORTED', $field_name, implode(' ', $bundle_strs) );
        }
    }
  }
  
  drush_print_table($rows);
  drush_print("\nYou can delete the MISSING fields using");
  drush_print("    drush field-delete <field-name> --bundle=<bundle>\n");  
  
}

