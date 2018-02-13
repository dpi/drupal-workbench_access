<?php

namespace Drupal\workbench_access;

use Drupal\Core\Entity\EntityTypeInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;
use Drupal\workbench_access\UserSectionStorageInterface;
use Drupal\workbench_access\RoleSectionStorageInterface;

/**
 * Provides the workbench access views integration.
 *
 * @internal
 */
class ViewsData {

  use StringTranslationTrait;

  /**
   * The entity type manager.
   *
   * @var \Drupal\Core\Entity\EntityTypeManagerInterface
   */
  protected $entityTypeManager;

  /**
   * The user section storage handler.
   *
   * @var \Drupal\workbench_access\UserSectionStorageInterface
   */
  protected $userSectionStorage;

  /**
   * The role section storage handler.
   *
   * @var \Drupal\workbench_access\RoleSectionStorageInterface
   */
  protected $roleSectionStorage;

  /**
   * Creates a new ViewsData instance.
   *
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   The entity type manager.
   * @param \Drupal\workbench_access\UserSectionStorageInterface $user_storage
   *   The user section storage handler.
   * @param \Drupal\workbench_access\RoleSectionStorageInterface $role_storage
   *   The role section storage handler.
   */
  public function __construct(EntityTypeManagerInterface $entity_type_manager, UserSectionStorageInterface $user_storage, RoleSectionStorageInterface $role_storage) {
    $this->entityTypeManager = $entity_type_manager;
    $this->userSectionStorage = $user_storage;
    $this->roleSectionStorage = $role_storage;
  }

  /**
   * Returns the views data.
   *
   * @return array
   *   The views data.
   */
  public function getViewsData() {
    $data = [];
    $scheme_storage = $this->entityTypeManager->getStorage('access_scheme');
    foreach ($scheme_storage->loadMultiple() as $scheme) {
      // Do things?

      /* Some notes for later:

        - Decide which tables to JOIN to, if any.
        - Decide which tables to form relationships for.
        - Define any configuration that will be necessary. I suspect that
          configuration per field will require specifying a specific scheme.
        - Explore whether the views_data_alter hook is still needed. In fact,
          we may need to do all of our computation there, with the exception
          of the base relationships.
        - However, if may be that the relationships require configuration.
      */
    }
    return $data;
  }

}
