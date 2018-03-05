<?php

namespace Drupal\workbench_access;

use Drupal\Core\Entity\Sql\SqlEntityStorageInterface;

/**
 * Defines section association storage.
 */
interface SectionAssociationStorageInterface extends SqlEntityStorageInterface {

  /**
   * Loads section information.
   *
   * A section is a dual-key index, so loading without an entity id requires
   * two keys.
   *
   * @param $access_scheme_id
   *   The id for an active access scheme entity.
   * @param $section_id
   *   The id for a section within the access scheme.
   *
   * @return \Drupal\workbench_access\Entity\SectionAssociation | NULL.
   */
  public function loadSection($access_scheme_id, $section_id);

}