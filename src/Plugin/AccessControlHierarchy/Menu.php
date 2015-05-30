<?php

/**
 * @file
 * Contains \Drupal\workbench_access\Plugin\AccessControlHierarchy\Menu.
 */

namespace Drupal\workbench_access\Plugin\AccessControlHierarchy;

use Drupal\workbench_access\AccessControlHierarchyBase;

/**
 * Defines a hierarchy based on a Menu.
 *
 * @AccessControlHierarchy(
 *   id = "menu",
 *   module = "menu_ui",
 *   base_entity = "menu",
 *   entity = "menu_link_content",
 *   label = @Translation("Menu"),
 *   description = @Translation("Uses a menu as an access control hierarchy.")
 * )
 */
class Menu extends AccessControlHierarchyBase {

}
