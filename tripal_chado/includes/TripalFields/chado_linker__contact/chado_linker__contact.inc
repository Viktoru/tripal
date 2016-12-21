<?php

class chado_linker__contact extends TripalField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The term that this field maps to.  The format for the term should be:
  // [vocab]:[accession] where [vocab] is the short name of the vocabulary
  // and [acession] is the unique accession number for the term.  This term
  // must already exist in the vocabulary storage backend. This
  // value should never be changed once fields exist for this type.
  public static $term = 'local:contact';

  // The default lable for this field.
  public static $label = 'Contacts';

  // The default description for this field.
  public static $description = 'Associates an indviddual or organization with
          this record';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  public static $instance_settings  = array();

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $storage = 'tripal_no_storage';

  // The default widget for this field.
  public static $default_widget = 'chado_linker__contact_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__contact_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;
  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   *
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

  }


  /**
   *
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {
    $record = $details['record'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    
    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $details['record']->tablename;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];
    
    
    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => '',
      'chado-' . $field_table . '__' . 'contact_id' => '',
      // Ignore the synonym_sgml column for now.
    );
    
    $linker_table = $base_table . '_contact';
    $options = array(
      'return_array' => 1,
      'include_fk' => array(
        'contact_id' => array(
          'type_id' => array(
            'dbxref_id' => array(
              'db_id' => TRUE,
            ),
          ),
        ),
        $fkey_lcolumn => TRUE,
      ),
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $contact_linkers = $record->$linker_table;
    if ($contact_linkers) {
      foreach ($contact_linkers as $i => $contact_linker) {
        $contact = $contact_linker->contact_id;
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            'type' => $contact->type_id ? $contact->type_id->name : '',
            'name' => $contact->name,
            'description' => $contact->description,
          ),
          // Add in the semantic web settings.  This array is expected by
          // other Tripal modules that handle semantic web for fields.
          'semantic_web' => array(
            'type' => $contact->type_id ? $contact->type_id->dbxref_id->db_id->name . ':' . $contact->type_id->dbxref_id->accession : '',
            'name' => tripal_get_chado_semweb_term('contact', 'name'),
            'description' => tripal_get_chado_semweb_term('contact', 'description'),
          ),
          // Add in subfield mapping to Chado tables. This is used by the
          // chado_field_storage for performing queries on sub element values.
          // It should be a comma-separated list (no spacing) of the field names
          // as foreign keys are followed starting from the Chado table to which
          // this field maps.
          'chado_mapping' => array(
            'type' => 'type_id,name',
            'name' => 'contact_id,name',
            'description' => 'contact_id,name'
          ),
          'chado-' . $field_table . '__' . $pkey => $contact_linker->$pkey,
          'chado-' . $field_table . '__' . $fkey_lcolumn => $contact_linker->$fkey_lcolumn->$fkey_lcolumn,
          'chado-' . $field_table . '__' . 'contact_id' => $contact->contact_id
        );
    
        if (property_exists($contact, 'entity_id')) {
          $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $contact->entity_id;
        }
      }
    }
  }


  /**
   *
   * @see TripalField::settingsForm()
   */
  public function settingsForm($has_data) {

  }
  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public function settingsFormValidate($form, &$form_state) {

  }

  /**
   *
   * @see TripalField::instanceSettingsForm()
   */
  public function instanceSettingsForm() {

  }
  /**
   *
   * @see TripalField::instanceSettingsFormValidate()
   */
  public function instanceSettingsFormValidate($form, &$form_state) {

  }

}

/**
 * An Ajax callback for the pub widget.
 */
function chado_linker__contact_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}
/**
 * Theme function for the pub widget.
 *
 * @param $variables
 */
function theme_chado_linker__contact_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"pub-widget\">
        <div class=\"pub-widget-item\">" .
        drupal_render($element['name']) . "
        </div>
      </div>
    ";

        return $layout;
}