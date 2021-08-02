CREATE TABLE `recurring_payment_contracts` (
    `id` bigint unsigned NOT NULL AUTO_INCREMENT,
    `user_id` bigint unsigned NOT NULL DEFAULT '0',
    `mch_id` bigint unsigned NOT NULL DEFAULT '0',
    `app_id` bigint unsigned NOT NULL DEFAULT '0',
    `plan_id` bigint unsigned NOT NULL DEFAULT '0',
    `contract_code` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '',
    `contract_state` tinyint unsigned NOT NULL DEFAULT '0',
    `display_account` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL,
    `signed_time` datetime DEFAULT NULL,
    `expired_time` datetime DEFAULT NULL,
    `terminated_time` datetime DEFAULT NULL,
    PRIMARY KEY (`id`) USING BTREE,
    UNIQUE KEY `mch_id` (`mch_id`,`contract_code`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;