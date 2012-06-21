<?php

/**
 *  @file
 *  This file contains the basic functions for views integration of
 *  chado/tripal_genetic tables. Supplementary functions can be found in
 *  ./views/
 *
 *  Documentation on views integration can be found at
 *  http://views2.logrus.com/doc/html/index.html.
 */

/*************************************************************************
 * Implements hook_views_data()
 * Purpose: Describe chado/tripal tables & fields to views
 *
 * @return: a data array which follows the structure outlined in the
 *   views2 documentation for this hook. Essentially, it's an array of table
 *   definitions keyed by chado/tripal table name. Each table definition
 *   includes basic details about the table, fields in that table and
 *   relationships between that table and others (joins)
 */
function tripal_genetic_views_data()  {
  $data = array();

  if (module_exists('tripal_views')) {
    $tables = array(
      'environment',
      'genotype',
      'phenstatement'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 10)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename,TRUE);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }

    $tables = array(
      'environment_cvterm',
      'feautre_genotype',
      'phendesc',
      'phenotype_comparison'
    );
    foreach ($tables as $tablename) {
      if (!tripal_views_is_integrated($tablename, 10)) {
        $table_integration_array = tripal_views_get_integration_array_for_chado_table($tablename,FALSE);
        tripal_views_integration_add_entry($table_integration_array);
      }
    }
  }

  return $data;
}

/*************************************************************************
 * Implements hook_views_handlers()
 * Purpose: Register all custom handlers with views
 *   where a handler describes either "the type of field",
 *   "how a field should be filtered", "how a field should be sorted"
 *
 * @return: An array of handler definitions
 */
function tripal_genetic_views_handlers() {
 return array(
   'info' => array(
     'path' => drupal_get_path('module', 'tripal_genetic') . '/views/handlers',
   ),
   'handlers' => array(

   ),
 );
}