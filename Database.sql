-- phpMyAdmin SQL Dump
-- version 5.2.3
-- https://www.phpmyadmin.net/
--
-- Hôte : 127.0.0.1:3306
-- Généré le : mer. 21 oct. 2026 à 20:44
-- Version du serveur : 8.4.7
-- Version de PHP : 8.3.28

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Base de données : `domotique`
--

-- --------------------------------------------------------

--
-- Structure de la table `climatisations`
--

DROP TABLE IF EXISTS `climatisations`;
CREATE TABLE IF NOT EXISTS `climatisations` (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `ip_automate` varchar(15) COLLATE utf8mb4_unicode_ci NOT NULL,
  `entree_automate` varchar(8) COLLATE utf8mb4_unicode_ci NOT NULL,
  `etat` enum('allumé','éteint','en panne') COLLATE utf8mb4_unicode_ci NOT NULL,
  `description` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `localisation_id` int UNSIGNED NOT NULL,
  `mode` tinyint(1) NOT NULL COMMENT '0 = climatisation, 1 = chauffage',
  PRIMARY KEY (`id`),
  KEY `idx_climatisations_localisation` (`localisation_id`)
) ENGINE=InnoDB AUTO_INCREMENT=31 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Déchargement des données de la table `climatisations`
--

INSERT INTO `climatisations` (`id`, `ip_automate`, `entree_automate`, `etat`, `description`, `localisation_id`, `mode`) VALUES
(1, '192.168.0.102', '00000001', 'allumé', 'Suite parentale - Climatisation réversible', 12, 1),
(2, '192.168.0.102', '00000010', 'allumé', 'Suite parentale - Salle de bain - Climatisation réversible', 13, 1),
(3, '192.168.0.102', '00000011', 'en panne', 'Suite parentale - Dressing - Climatisation réversible', 14, 1),
(4, '192.168.0.102', '00000100', 'allumé', 'Chambre invités - Climatisation réversible', 9, 1),
(5, '192.168.0.102', '00000101', 'allumé', 'Chambre invités - Salle de bain - Climatisation réversible', 10, 1),
(6, '192.168.0.102', '00000110', 'allumé', 'Chambre invités - Dressing - Climatisation réversible', 11, 1),
(7, '192.168.0.102', '00000111', 'éteint', 'Garages nord - Climatisation réversible', 19, 1),
(8, '192.168.0.102', '00001000', 'éteint', 'Garages ouest - Climatisation réversible', 20, 1),
(9, '192.168.0.102', '00001001', 'allumé', 'WC - Climatisation réversible', 24, 1),
(10, '192.168.0.102', '00001010', 'allumé', 'Salon - Climatisation réversible', 23, 1),
(11, '192.168.0.102', '00001011', 'allumé', 'Hall nord - Climatisation réversible', 21, 1),
(12, '192.168.0.102', '00001100', 'allumé', 'Escalier central - Climatisation réversible', 17, 1),
(13, '192.168.0.102', '00001101', 'allumé', 'Cuisine - Climatisation réversible', 16, 1),
(14, '192.168.0.102', '00001110', 'allumé', 'Salle à manger - Climatisation réversible', 22, 1),
(15, '192.168.0.102', '00001111', 'allumé', 'Bibliothèque - Climatisation réversible', 8, 1),
(16, '192.168.0.112', '00000001', 'allumé', 'Bureau - Climatisation réversible', 29, 1),
(17, '192.168.0.112', '00000010', 'allumé', 'Chambre nord est - Climatisation réversible', 30, 1),
(18, '192.168.0.112', '00000011', 'allumé', 'Chambre nord est - Salle de bain - Climatisation réversible', 31, 1),
(19, '192.168.0.112', '00000100', 'allumé', 'Chambre sud est - Climatisation réversible', 35, 1),
(20, '192.168.0.112', '00000101', 'allumé', 'Chambre sud est - Salle de bain - Climatisation réversible', 37, 1),
(21, '192.168.0.112', '00000110', 'allumé', 'Chambre sud est - Dressing - Climatisation réversible', 36, 1),
(22, '192.168.0.112', '00000111', 'allumé', 'Chambre sud ouest - Climatisation réversible', 38, 1),
(23, '192.168.0.112', '00001000', 'allumé', 'Chambre sud ouest - Dressing - Climatisation réversible', 39, 1),
(24, '192.168.0.112', '00001001', 'en panne', 'Chambre sud ouest - Salle de bain - Climatisation réversible', 40, 1),
(25, '192.168.0.112', '00001010', 'allumé', 'Salle de jeux - Climatisation réversible', 45, 1),
(26, '192.168.0.112', '00001011', 'allumé', 'WC - Climatisation réversible', 26, 1),
(27, '192.168.0.112', '00001100', 'allumé', 'Chambre nord ouest - Climatisation réversible', 32, 1),
(28, '192.168.0.112', '00001101', 'allumé', 'Chambre nord ouest - Dressing - Climatisation réversible', 33, 1),
(29, '192.168.0.112', '00001110', 'allumé', 'Chambre nord ouest - Salle de bain - Climatisation réversible', 34, 1),
(30, '192.168.0.112', '00001111', 'en panne', 'Couloir circulaire - Climatisation réversible', 42, 1);

-- --------------------------------------------------------

--
-- Structure de la table `localisations`
--

DROP TABLE IF EXISTS `localisations`;
CREATE TABLE IF NOT EXISTS `localisations` (
  `id` int UNSIGNED NOT NULL,
  `description` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Déchargement des données de la table `localisations`
--

INSERT INTO `localisations` (`id`, `description`) VALUES
(1, 'Extérieur - Terrasse'),
(2, 'Extérieur - Entrées terrasse'),
(3, 'Extérieur - Hall sud'),
(4, 'Extérieur - Piscine'),
(5, 'Extérieur - Entrée garages ouest'),
(6, 'Extérieur - Entrée garages nord'),
(7, 'Extérieur - Entrée véhicules garages'),
(8, 'Rez-de-chaussée - Bibliothèque'),
(9, 'Rez-de-chaussée - Chambre invités'),
(10, 'Rez-de-chaussée - Chambre invités - Salle de bain'),
(11, 'Rez-de-chaussée - Chambre invités - Dressing'),
(12, 'Rez-de-chaussée - Suite parentale'),
(13, 'Rez-de-chaussée - Suite parentale - Salle de bain'),
(14, 'Rez-de-chaussée - Suite parentale - Dressing'),
(15, 'Rez-de-chaussée - Vestibule'),
(16, 'Rez-de-chaussée - Cuisine'),
(17, 'Rez-de-chaussée - Escalier central'),
(18, 'Rez-de-chaussée - Escalier est'),
(19, 'Rez-de-chaussée - Garages nord'),
(20, 'Rez-de-chaussée - Garages ouest'),
(21, 'Rez-de-chaussée - Hall nord'),
(22, 'Rez-de-chaussée - Salle à manger'),
(23, 'Rez-de-chaussée - Salon'),
(24, 'Rez-de-chaussée - WC'),
(25, 'Premier étage - Placard'),
(26, 'Premier étage - WC'),
(27, 'Premier étage - Accès au grenier'),
(28, 'Premier étage - Balcon'),
(29, 'Premier étage - Bureau'),
(30, 'Premier étage - Chambre nord est'),
(31, 'Premier étage - Chambre nord est - Salle de bain'),
(32, 'Premier étage - Chambre nord ouest'),
(33, 'Premier étage - Chambre nord ouest - Dressing'),
(34, 'Premier étage - Chambre nord ouest - Salle de bain'),
(35, 'Premier étage - Chambre sud est'),
(36, 'Premier étage - Chambre sud est - Dressing'),
(37, 'Premier étage - Chambre sud est - Salle de bain'),
(38, 'Premier étage - Chambre sud ouest'),
(39, 'Premier étage - Chambre sud ouest - Dressing'),
(40, 'Premier étage - Chambre sud ouest - Salle de bain'),
(41, 'Premier étage - Escalier central'),
(42, 'Premier étage - Couloir circulaire'),
(43, 'Premier étage - Escalier est'),
(44, 'Premier étage - Hall nord'),
(45, 'Premier étage - Salle de jeux'),
(200, 'Rez-de-chaussée - Placard');

-- --------------------------------------------------------

--
-- Structure de la table `luminaires`
--

DROP TABLE IF EXISTS `luminaires`;
CREATE TABLE IF NOT EXISTS `luminaires` (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `ip_automate` varchar(15) COLLATE utf8mb4_unicode_ci NOT NULL,
  `entree_automate` varchar(8) COLLATE utf8mb4_unicode_ci NOT NULL,
  `etat` enum('allumé','éteint','en panne') COLLATE utf8mb4_unicode_ci NOT NULL,
  `description` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `localisation_id` int UNSIGNED NOT NULL,
  `puissance_w` decimal(6,1) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_luminaires_localisation` (`localisation_id`)
) ENGINE=InnoDB AUTO_INCREMENT=124 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Déchargement des données de la table `luminaires`
--

INSERT INTO `luminaires` (`id`, `ip_automate`, `entree_automate`, `etat`, `description`, `localisation_id`, `puissance_w`) VALUES
(1, '192.168.0.100', '00111110', 'éteint', 'Garages nord - Projecteur extérieur entrée véhicule nord', 6, 500.0),
(2, '192.168.0.100', '00111111', 'éteint', 'Garages nord - Projecteur extérieur entrée véhicule sud', 6, 500.0),
(3, '192.168.0.100', '01000000', 'éteint', 'Garages nord - Hublot entrée est', 6, 200.0),
(4, '192.168.0.100', '01000001', 'éteint', 'Garages ouest - Projecteur extérieur entrée véhicule est', 5, 500.0),
(5, '192.168.0.100', '01000010', 'éteint', 'Garages ouest - Projecteur extérieur entrée véhicule ouest', 5, 500.0),
(6, '192.168.0.100', '01000011', 'éteint', 'Garages ouest - Hublot entrée ouest', 5, 200.0),
(7, '192.168.0.100', '01000100', 'éteint', 'Terrasse - Hublot porte chambre invités', 2, 200.0),
(8, '192.168.0.100', '01000101', 'éteint', 'Terrasse - Hublot porte cuisine', 2, 200.0),
(9, '192.168.0.100', '01000110', 'éteint', 'Terrasse - Hublot cuisine d\'été', 1, 200.0),
(10, '192.168.0.100', '01000111', 'éteint', 'Terrasse - Ensemble de spots immergés piscine', 4, 200.0),
(11, '192.168.0.100', '01001000', 'éteint', 'Hall sud - Hublot ouest', 3, 200.0),
(12, '192.168.0.100', '01001001', 'éteint', 'Hall sud - Hublot est', 3, 200.0),
(13, '192.168.0.100', '01001010', 'éteint', 'Hall sud - Projecteur extérieur ouest', 3, 300.0),
(14, '192.168.0.100', '01001011', 'éteint', 'Hall sud - Projecteur extérieur est', 3, 300.0),
(15, '192.168.0.100', '00000001', 'éteint', 'Cuisine - Luminaire entrée', 16, 100.0),
(16, '192.168.0.100', '00000010', 'éteint', 'Cuisine - Luminaire îlot central', 16, 100.0),
(17, '192.168.0.100', '00000011', 'éteint', 'Cuisine - Plan de travail est (gauche)', 16, 100.0),
(18, '192.168.0.100', '00000100', 'éteint', 'Cuisine - Plan de travail est (droit)', 16, 100.0),
(19, '192.168.0.100', '00000101', 'éteint', 'Cuisine - Plan de travail ouest (droite)', 16, 100.0),
(20, '192.168.0.100', '00000110', 'éteint', 'Cuisine - Plan de travail ouest (gauche)', 16, 100.0),
(21, '192.168.0.100', '00000111', 'éteint', 'Cuisine - Plan de travail sud', 16, 100.0),
(22, '192.168.0.100', '00001000', 'éteint', 'Suite parentale - Luminaire central', 12, 100.0),
(23, '192.168.0.100', '00001001', 'éteint', 'Suite parentale - Applique nord-ouest', 12, 100.0),
(24, '192.168.0.100', '00001010', 'éteint', 'Suite parentale - Applique nord-est', 12, 100.0),
(25, '192.168.0.100', '00001011', 'éteint', 'Suite parentale - Applique sud-ouest', 12, 100.0),
(26, '192.168.0.100', '00001100', 'éteint', 'Suite parentale - Applique sud', 12, 100.0),
(27, '192.168.0.100', '00001101', 'éteint', 'Suite parentale - Salle de bain et dressing - Luminaire entrée salle de bain', 10, 100.0),
(28, '192.168.0.100', '00001110', 'éteint', 'Suite parentale - Salle de bain et dressing - Luminaire salle de bain central', 10, 100.0),
(29, '192.168.0.100', '00001111', 'éteint', 'Suite parentale - Salle de bain et dressing - Baignoire', 10, 100.0),
(30, '192.168.0.100', '00010000', 'éteint', 'Suite parentale - Salle de bain et dressing - Lavabo nord', 10, 100.0),
(31, '192.168.0.100', '00010001', 'éteint', 'Suite parentale - Salle de bain et dressing - Lavabo est', 10, 100.0),
(32, '192.168.0.100', '00010010', 'éteint', 'Suite parentale - Salle de bain et dressing - Luminaire douche', 10, 100.0),
(33, '192.168.0.100', '00010011', 'éteint', 'Suite parentale - Salle de bain et dressing - Luminaire WC', 10, 100.0),
(34, '192.168.0.100', '00010100', 'éteint', 'Suite parentale - Salle de bain et dressing - Luminaire nord dressing', 11, 100.0),
(35, '192.168.0.100', '00010101', 'éteint', 'Suite parentale - Salle de bain et dressing - Luminaire sud dressing', 11, 100.0),
(36, '192.168.0.100', '00010110', 'éteint', 'Suite parentale - Vestibule - Luminaire vestibule', 15, 100.0),
(37, '192.168.0.100', '00010111', 'allumé', 'Salon - Luminaire salon nord', 23, 100.0),
(38, '192.168.0.100', '00011000', 'allumé', 'Salon - Luminaire salon sud', 23, 100.0),
(39, '192.168.0.100', '00011001', 'éteint', 'Salon - Applique nord', 23, 100.0),
(40, '192.168.0.100', '00011010', 'allumé', 'Salon - Applique cheminée nord', 23, 100.0),
(41, '192.168.0.100', '00011011', 'allumé', 'Salon - Applique cheminée sud', 23, 100.0),
(42, '192.168.0.100', '00011100', 'éteint', 'Salon - Applique sud', 23, 100.0),
(43, '192.168.0.100', '00011101', 'allumé', 'Salon - Applique grand mur nord', 23, 100.0),
(44, '192.168.0.100', '00011110', 'allumé', 'Salon - Applique grand mur sud', 23, 100.0),
(45, '192.168.0.100', '00011111', 'éteint', 'Salle à manger - Luminaire central', 22, 100.0),
(46, '192.168.0.100', '00100000', 'éteint', 'Salle à manger - Applique nord ouest', 22, 100.0),
(47, '192.168.0.100', '00100001', 'éteint', 'Salle à manger - Applique nord est', 22, 100.0),
(48, '192.168.0.100', '00100010', 'éteint', 'Salle à manger - Applique nord mur ouest', 22, 100.0),
(49, '192.168.0.100', '00100011', 'en panne', 'Salle à manger - Applique sud mur ouest', 22, 100.0),
(50, '192.168.0.100', '00100100', 'éteint', 'Salle à manger - Applique sud-est', 22, 100.0),
(51, '192.168.0.100', '00100101', 'éteint', 'Placard - Applique', 200, 100.0),
(52, '192.168.0.100', '00100110', 'en panne', 'WC - Luminaire central', 24, 100.0),
(53, '192.168.0.100', '00100111', 'allumé', 'Escalier central - Luminaire central', 17, 100.0),
(54, '192.168.0.100', '00101000', 'allumé', 'Escalier central - Luminaire sud', 17, 100.0),
(55, '192.168.0.100', '00101001', 'allumé', 'Bibliothèque - Luminaire central ouest', 8, 100.0),
(56, '192.168.0.100', '00101010', 'éteint', 'Bibliothèque - Luminaire central est', 8, 100.0),
(57, '192.168.0.100', '00101011', 'éteint', 'Chambre invités - Luminaire central', 9, 100.0),
(58, '192.168.0.100', '00101100', 'éteint', 'Chambre invités - Applique ouest', 9, 100.0),
(59, '192.168.0.100', '00101101', 'éteint', 'Chambre invités - Applique sud-ouest', 9, 100.0),
(60, '192.168.0.100', '00101110', 'éteint', 'Chambre invités - Applique sud', 9, 100.0),
(61, '192.168.0.100', '00101111', 'éteint', 'Chambre invités - Salle de bain et dressing - Luminaire central salle de bain', 10, 100.0),
(62, '192.168.0.100', '00110000', 'éteint', 'Chambre invités - Salle de bain et dressing - Luminaire central dressing', 11, 100.0),
(63, '192.168.0.100', '00110001', 'éteint', 'Chambre invités - Salle de bain et dressing - Applique sud salle de bain', 10, 100.0),
(64, '192.168.0.100', '00110010', 'éteint', 'Chambre invités - Salle de bain et dressing - Luminaire central douche', 10, 100.0),
(65, '192.168.0.100', '00110011', 'éteint', 'Hall nord - Luminaire central', 21, 100.0),
(66, '192.168.0.100', '00110100', 'éteint', 'Hall nord - Applique ouest', 21, 100.0),
(67, '192.168.0.100', '00110101', 'éteint', 'Hall nord - Applique est', 21, 100.0),
(68, '192.168.0.100', '00110110', 'éteint', 'Garages nord - Luminaire central nord-ouest', 19, 100.0),
(69, '192.168.0.100', '00110111', 'éteint', 'Garages nord - Luminaire central nord-est', 19, 100.0),
(70, '192.168.0.100', '00111000', 'éteint', 'Garages nord - Luminaire central sud-ouest', 19, 100.0),
(71, '192.168.0.100', '00111001', 'en panne', 'Garages nord - Luminaire central sud-est', 19, 100.0),
(72, '192.168.0.100', '00111010', 'éteint', 'Garages ouest - Luminaire central nord-ouest', 20, 100.0),
(73, '192.168.0.100', '00111011', 'éteint', 'Garages ouest - Luminaire central nord-est', 20, 100.0),
(74, '192.168.0.100', '00111100', 'éteint', 'Garages ouest - Luminaire central sud-ouest', 20, 100.0),
(75, '192.168.0.100', '00111101', 'éteint', 'Garages ouest - Luminaire central sud-est', 20, 100.0),
(76, '192.168.0.110', '00000001', 'éteint', 'Bureau - Luminaire central', 29, 100.0),
(77, '192.168.0.110', '00000010', 'éteint', 'Chambre nord est - Luminaire central', 30, 100.0),
(78, '192.168.0.110', '00000011', 'allumé', 'Chambre nord est - Salle de bain - Luminaire central', 30, 100.0),
(79, '192.168.0.110', '00010001', 'allumé', 'Chambre nord est - Applique nord', 30, 100.0),
(80, '192.168.0.110', '00010010', 'allumé', 'Chambre nord est - Applique sud', 30, 100.0),
(81, '192.168.0.110', '00010011', 'éteint', 'Chambre nord est - Salle de bain - Lavabo ouest', 31, 100.0),
(82, '192.168.0.110', '00010100', 'éteint', 'Chambre nord est - Salle de bain - Lavabo est', 31, 100.0),
(83, '192.168.0.110', '00010101', 'éteint', 'Chambre nord est - Salle de bain - Baignoire', 31, 100.0),
(84, '192.168.0.110', '00000100', 'éteint', 'Escalier est - Luminaire central', 43, 100.0),
(85, '192.168.0.110', '00010110', 'éteint', 'Escalier est - Applique ouest', 43, 100.0),
(86, '192.168.0.110', '00010111', 'éteint', 'Escalier est - Applique est', 43, 100.0),
(87, '192.168.0.110', '00000101', 'éteint', 'Chambre sud est - Luminaire central', 35, 100.0),
(88, '192.168.0.110', '00000110', 'éteint', 'Chambre sud est - Dressing - Luminaire central', 36, 100.0),
(89, '192.168.0.110', '00000111', 'éteint', 'Chambre sud est - Salle de bain - Luminaire central', 37, 100.0),
(90, '192.168.0.110', '00011000', 'éteint', 'Chambre sud est - Applique ouest', 35, 100.0),
(91, '192.168.0.110', '00011001', 'éteint', 'Chambre sud est - Applique est', 35, 100.0),
(92, '192.168.0.110', '00011010', 'éteint', 'Chambre sud est - Salle de bain - Lavabo', 37, 100.0),
(93, '192.168.0.110', '00011011', 'éteint', 'Chambre sud est - Salle de bain - Baignoire', 37, 100.0),
(94, '192.168.0.110', '00001000', 'éteint', 'Chambre sud ouest - Luminaire central', 38, 100.0),
(95, '192.168.0.110', '00001001', 'éteint', 'Chambre sud ouest - Dressing - Luminaire central', 39, 100.0),
(96, '192.168.0.110', '00001010', 'éteint', 'Chambre sud ouest - Salle de bain - Luminaire central', 40, 100.0),
(97, '192.168.0.110', '00100010', 'éteint', 'Chambre sud ouest - Applique ouest', 38, 100.0),
(98, '192.168.0.110', '00100011', 'éteint', 'Chambre sud ouest - Applique est', 38, 100.0),
(99, '192.168.0.110', '00100100', 'éteint', 'Chambre sud ouest - Salle de bain - Lavabo', 40, 100.0),
(100, '192.168.0.110', '00100101', 'éteint', 'Chambre sud ouest - Salle de bain - Baignoire', 40, 100.0),
(101, '192.168.0.110', '00001011', 'éteint', 'Salle de jeux - Luminaire central nord', 45, 100.0),
(102, '192.168.0.110', '00001100', 'éteint', 'Salle de jeux - Luminaire central sud', 45, 100.0),
(103, '192.168.0.110', '00011100', 'éteint', 'Salle de jeux - Applique nord ouest', 45, 100.0),
(104, '192.168.0.110', '00011101', 'éteint', 'Salle de jeux - Applique nord est', 45, 100.0),
(105, '192.168.0.110', '00011110', 'éteint', 'Salle de jeux - Applique ouest', 45, 100.0),
(106, '192.168.0.110', '00011111', 'éteint', 'Salle de jeux - Applique sud ouest', 45, 100.0),
(107, '192.168.0.110', '00100000', 'éteint', 'Salle de jeux - Applique sud est', 45, 100.0),
(108, '192.168.0.110', '00100001', 'éteint', 'Salle de jeux - Applique est', 45, 100.0),
(109, '192.168.0.110', '00001101', 'éteint', 'Chambre nord ouest - Luminaire central', 32, 100.0),
(110, '192.168.0.110', '00001110', 'éteint', 'Chambre nord ouest - Dressing - Luminaire central', 33, 100.0),
(111, '192.168.0.110', '00001111', 'éteint', 'Chambre nord ouest - Salle de bain - Luminaire central', 34, 100.0),
(112, '192.168.0.110', '00101000', 'éteint', 'Chambre nord ouest - Applique nord', 32, 100.0),
(113, '192.168.0.110', '00101001', 'éteint', 'Chambre nord ouest - Applique sud', 32, 100.0),
(114, '192.168.0.110', '00101010', 'éteint', 'Chambre nord ouest - Salle de bain - Lavabo', 34, 100.0),
(115, '192.168.0.110', '00101011', 'éteint', 'Chambre nord ouest - Salle de bain - Baignoire', 34, 100.0),
(116, '192.168.0.110', '00100110', 'éteint', 'WC - Lavabo', 26, 100.0),
(117, '192.168.0.110', '00010000', 'éteint', 'WC - Luminaire central', 26, 100.0),
(118, '192.168.0.110', '00100111', 'éteint', 'Accès au grenier - Applique', 27, 100.0),
(119, '192.168.0.110', '00101100', 'éteint', 'Couloir circulaire - Applique ouest', 42, 100.0),
(120, '192.168.0.110', '00101101', 'éteint', 'Couloir circulaire - Applique est', 42, 100.0),
(121, '192.168.0.110', '00101110', 'éteint', 'Couloir circulaire - Applique sud', 42, 100.0),
(122, '192.168.0.110', '00101111', 'éteint', 'Balcon - Applique ouest', 28, 100.0),
(123, '192.168.0.110', '00110000', 'éteint', 'Balcon - Applique est', 28, 100.0);

-- --------------------------------------------------------

--
-- Structure de la table `prises`
--

DROP TABLE IF EXISTS `prises`;
CREATE TABLE IF NOT EXISTS `prises` (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `ip_automate` varchar(15) COLLATE utf8mb4_unicode_ci NOT NULL,
  `entree_automate` varchar(8) COLLATE utf8mb4_unicode_ci NOT NULL,
  `etat` enum('allumé','éteint','en panne') COLLATE utf8mb4_unicode_ci NOT NULL,
  `description` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `localisation_id` int UNSIGNED NOT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_prises_localisation` (`localisation_id`)
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Déchargement des données de la table `prises`
--

INSERT INTO `prises` (`id`, `ip_automate`, `entree_automate`, `etat`, `description`, `localisation_id`) VALUES
(1, '192.168.0.101', '00000001', 'allumé', 'Cuisine - Prise ouest porte terrasse', 16),
(2, '192.168.0.101', '00000010', 'allumé', 'Cuisine - Prise est porte terrasse', 16),
(3, '192.168.0.101', '00000011', 'allumé', 'Cuisine - Prise nord îlot central', 16),
(4, '192.168.0.101', '00000100', 'allumé', 'Cuisine - Prise sud îlot central', 16);

-- --------------------------------------------------------

--
-- Structure de la table `volets`
--

DROP TABLE IF EXISTS `volets`;
CREATE TABLE IF NOT EXISTS `volets` (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `ip_automate` varchar(15) COLLATE utf8mb4_unicode_ci NOT NULL,
  `entree_automate` varchar(8) COLLATE utf8mb4_unicode_ci NOT NULL,
  `etat` enum('allumé','éteint','en panne') COLLATE utf8mb4_unicode_ci NOT NULL,
  `description` varchar(255) COLLATE utf8mb4_unicode_ci NOT NULL,
  `localisation_id` int UNSIGNED NOT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_volets_localisation` (`localisation_id`)
) ENGINE=InnoDB AUTO_INCREMENT=55 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

--
-- Déchargement des données de la table `volets`
--

INSERT INTO `volets` (`id`, `ip_automate`, `entree_automate`, `etat`, `description`, `localisation_id`) VALUES
(1, '192.168.0.103', '00000001', 'allumé', 'Hall nord - Volet roulant grande baie vitrée', 21),
(2, '192.168.0.103', '00000010', 'allumé', 'Garages nord - Volet roulant fenêtre est', 19),
(3, '192.168.0.103', '00000011', 'allumé', 'Garages nord - Volet roulant fenêtre nord-est', 19),
(4, '192.168.0.103', '00000100', 'allumé', 'Garages nord - Volet roulant fenêtre nord', 19),
(5, '192.168.0.103', '00000101', 'allumé', 'Garages nord - Volet roulant fenêtre nord-ouest', 19),
(6, '192.168.0.103', '00000110', 'éteint', 'Garages nord - Porte basculante nord', 19),
(7, '192.168.0.103', '00000111', 'éteint', 'Garages nord - Porte basculante sud', 19),
(8, '192.168.0.103', '00001000', 'allumé', 'Garages ouest - Volet roulant fenêtre est', 20),
(9, '192.168.0.103', '00001001', 'éteint', 'Garages ouest - Porte basculante est', 20),
(10, '192.168.0.103', '00001010', 'éteint', 'Garages ouest - Porte basculante ouest', 20),
(11, '192.168.0.103', '00001011', 'allumé', 'Garages ouest - Volet roulant bow window ouest fenêtre nord', 20),
(12, '192.168.0.103', '00001100', 'allumé', 'Garages ouest - Volet roulant bow window ouest fenêtre ouest', 20),
(13, '192.168.0.103', '00001101', 'allumé', 'Garages ouest - Volet roulant bow window ouest fenêtre sud', 20),
(14, '192.168.0.103', '00001110', 'allumé', 'Suite parentale - Volet roulant bow window ouest fenêtre nord', 12),
(15, '192.168.0.103', '00001111', 'allumé', 'Suite parentale - Volet roulant bow window ouest fenêtre ouest', 12),
(16, '192.168.0.103', '00010000', 'allumé', 'Suite parentale - Volet roulant bow window ouest fenêtre sud', 12),
(17, '192.168.0.103', '00010001', 'allumé', 'Suite parentale - Volet roulant bow window sud fenêtre ouest', 12),
(18, '192.168.0.103', '00010010', 'allumé', 'Suite parentale - Volet roulant bow window sud fenêtre sud', 12),
(19, '192.168.0.103', '00010011', 'allumé', 'Suite parentale - Volet roulant bow window sud fenêtre est', 12),
(20, '192.168.0.103', '00010100', 'allumé', 'Suite parentale - Salle de bain - Volet roulant bow window fenêtre nord', 13),
(21, '192.168.0.103', '00010101', 'allumé', 'Suite parentale - Salle de bain - Volet roulant bow window fenêtre ouest', 13),
(22, '192.168.0.103', '00010110', 'allumé', 'Suite parentale - Salle de bain - Volet roulant bow window fenêtre sud', 13),
(23, '192.168.0.103', '00010111', 'allumé', 'Salon - Volet roulant fenêtre sud', 23),
(24, '192.168.0.103', '00011000', 'allumé', 'Salon - Volet roulant fenêtre sud-est', 23),
(25, '192.168.0.103', '00011001', 'allumé', 'Escalier central - Volet roulant fenêtre ouest', 17),
(26, '192.168.0.103', '00011010', 'allumé', 'Escalier central - Volet roulant fenêtre est', 17),
(27, '192.168.0.103', '00011011', 'allumé', 'Bibliothèque - Volet roulant fenêtre sud-ouest', 8),
(28, '192.168.0.103', '00011100', 'allumé', 'Bibliothèque - Volet roulant fenêtre sud', 8),
(29, '192.168.0.103', '00011101', 'allumé', 'Salle à manger - Volet roulant bow window sud fenêtre ouest', 22),
(30, '192.168.0.103', '00011110', 'allumé', 'Salle à manger - Volet roulant bow window sud fenêtre sud', 22),
(31, '192.168.0.103', '00011111', 'en panne', 'Salle à manger - Volet roulant bow window sud fenêtre est', 22),
(32, '192.168.0.103', '00100000', 'allumé', 'Salle à manger - Volet roulant bow window est fenêtre sud', 22),
(33, '192.168.0.103', '00100001', 'allumé', 'Salle à manger - Volet roulant bow window est fenêtre est', 22),
(34, '192.168.0.103', '00100010', 'allumé', 'Salle à manger - Volet roulant bow window est fenêtre nord', 22),
(35, '192.168.0.103', '00100011', 'allumé', 'Chambre invités - Volet roulant fenêtre est', 9),
(36, '192.168.0.103', '00100100', 'allumé', 'Chambre invités - Volet roulant fenêtre nord', 9),
(37, '192.168.0.103', '00100101', 'allumé', 'Chambre invités - Volet roulant porte fenêtre terrasse', 9),
(38, '192.168.0.103', '00100110', 'allumé', 'Chambre invités - Salle de bain - Volet roulant bow window fenêtre sud', 10),
(39, '192.168.0.103', '00100111', 'allumé', 'Chambre invités - Salle de bain - Volet roulant bow window fenêtre est', 10),
(40, '192.168.0.103', '00101000', 'allumé', 'Chambre invités - Salle de bain - Volet roulant bow window fenêtre nord', 10),
(41, '192.168.0.103', '00101001', 'allumé', 'Cuisine - Volet roulant porte fenêtre terrasse', 16),
(42, '192.168.0.113', '00000001', 'allumé', 'Bureau - Volet roulant fenêtre nord', 29),
(43, '192.168.0.113', '00000010', 'allumé', 'Chambre NE - Volet roulant fenêtre nord', 30),
(44, '192.168.0.113', '00000011', 'allumé', 'Chambre NE - Volet roulant fenêtre est', 30),
(45, '192.168.0.113', '00000100', 'allumé', 'Chambre SE - Volet roulant fenêtre sud', 35),
(46, '192.168.0.113', '00000101', 'allumé', 'Chambre SE - Volet roulant fenêtre sud-ouest', 35),
(47, '192.168.0.113', '00000110', 'allumé', 'Balcon - Volet roulant fenêtre est', 28),
(48, '192.168.0.113', '00000111', 'allumé', 'Balcon - Volet roulant fenêtre centrale', 28),
(49, '192.168.0.113', '00001000', 'allumé', 'Balcon - Volet roulant fenêtre ouest', 28),
(50, '192.168.0.113', '00001001', 'allumé', 'Chambre SO - Volet roulant fenêtre sud-est', 38),
(51, '192.168.0.113', '00001010', 'allumé', 'Chambre SO - Volet roulant fenêtre sud', 38),
(52, '192.168.0.113', '00001011', 'allumé', 'Salle de jeux - Volet roulant fenêtre ouest', 45),
(53, '192.168.0.113', '00001100', 'allumé', 'Chambre NO - Volet roulant fenêtre nord', 32),
(54, '192.168.0.113', '00001101', 'allumé', 'Hall nord - Volet roulant grande baie vitrée', 44);

--
-- Contraintes pour les tables déchargées
--

--
-- Contraintes pour la table `climatisations`
--
ALTER TABLE `climatisations`
  ADD CONSTRAINT `fk_climatisations_localisation` FOREIGN KEY (`localisation_id`) REFERENCES `localisations` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Contraintes pour la table `luminaires`
--
ALTER TABLE `luminaires`
  ADD CONSTRAINT `fk_luminaires_localisation` FOREIGN KEY (`localisation_id`) REFERENCES `localisations` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Contraintes pour la table `prises`
--
ALTER TABLE `prises`
  ADD CONSTRAINT `fk_prises_localisation` FOREIGN KEY (`localisation_id`) REFERENCES `localisations` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Contraintes pour la table `volets`
--
ALTER TABLE `volets`
  ADD CONSTRAINT `fk_volets_localisation` FOREIGN KEY (`localisation_id`) REFERENCES `localisations` (`id`) ON DELETE CASCADE ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
