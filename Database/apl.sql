-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Creato il: Feb 29, 2024 alle 22:55
-- Versione del server: 10.4.32-MariaDB
-- Versione PHP: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `apl`
--
CREATE DATABASE IF NOT EXISTS `apl` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `apl`;

-- --------------------------------------------------------

--
-- Struttura della tabella `legenda`
--

CREATE TABLE `legenda` (
  `id` int(11) NOT NULL,
  `nome` varchar(50) NOT NULL,
  `descrizione` varchar(300) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dump dei dati per la tabella `legenda`
--

INSERT INTO `legenda` (`id`, `nome`, `descrizione`) VALUES
(1, 'Equalizzazione cella 0', '1 attivata, 0 disattivata'),
(2, 'Equalizzazione cella 1', '1 attivata, 0 disattivata'),
(3, 'Equalizzazione cella 2', '1 attivata, 0 disattivata'),
(4, 'Equalizzazione cella 3', '1 attivata, 0 disattivata'),
(5, 'Equalizzazione cella 4', '1 attivata, 0 disattivata'),
(6, 'Equalizzazione cella 5', '1 attivata, 0 disattivata'),
(7, 'Equalizzazione cella 6', '1 attivata, 0 disattivata'),
(8, 'Equalizzazione cella 7	', '1 attivata, 0 disattivata'),
(9, 'Equalizzazione cella 8', '1 attivata, 0 disattivata'),
(10, 'Equalizzazione cella 9', '1 attivata, 0 disattivata'),
(11, 'Equalizzazione cella 10', '1 attivata, 0 disattivata'),
(12, 'Equalizzazione cella 11', '1 attivata, 0 disattivata'),
(13, 'Equalizzazione cella 12', '1 attivata, 0 disattivata'),
(14, 'Equalizzazione cella 13', '1 attivata, 0 disattivata'),
(15, 'Equalizzazione cella 14', '1 attivata, 0 disattivata'),
(16, 'Equalizzazione cella 15', '1 attivata, 0 disattivata'),
(17, 'Equalizzazione cella 16', '1 attivata, 0 disattivata'),
(18, 'Equalizzazione cella 17', '1 attivata, 0 disattivata'),
(19, 'Equalizzazione cella 18', '1 attivata, 0 disattivata'),
(20, 'Equalizzazione cella 19', '1 attivata, 0 disattivata'),
(21, 'Equalizzazione cella 20', '1 attivata, 0 disattivata'),
(22, 'Equalizzazione cella 21', '1 attivata, 0 disattivata'),
(23, 'Equalizzazione cella 22', '1 attivata, 0 disattivata'),
(24, 'Equalizzazione cella 23', '1 attivata, 0 disattivata'),
(25, 'Equalizzazione cella 24', '1 attivata, 0 disattivata'),
(26, 'Equalizzazione cella 25', '1 attivata, 0 disattivata'),
(27, 'Equalizzazione cella 26', '1 attivata, 0 disattivata'),
(28, 'Equalizzazione cella 27', '1 attivata, 0 disattivata'),
(29, 'Equalizzazione cella 28', '1 attivata, 0 disattivata'),
(30, 'Equalizzazione cella 29', '1 attivata, 0 disattivata'),
(31, 'Equalizzazione cella 30', '1 attivata, 0 disattivata'),
(32, 'Equalizzazione cella 31', '1 attivata, 0 disattivata'),
(33, 'Tensione cella 0', 'In mV'),
(34, 'Tensione cella 1', 'In mV'),
(35, 'Tensione cella 2', 'In mV'),
(36, 'Tensione cella 3', 'In mV'),
(37, 'Tensione cella 4', 'In mV'),
(38, 'Tensione cella 5', 'In mV'),
(39, 'Tensione cella 6', 'In mV'),
(40, 'Tensione cella 7', 'In mV'),
(41, 'Tensione cella 8', 'In mV'),
(42, 'Tensione cella 9', 'In mV'),
(43, 'Tensione cella 10', 'In mV'),
(44, 'Tensione cella 11', 'In mV'),
(45, 'Tensione cella 12', 'In mV'),
(46, 'Tensione cella 13', 'In mV'),
(47, 'Tensione cella 14', 'In mV'),
(48, 'Tensione cella 15', 'In mV'),
(49, 'Tensione cella 16', 'In mV'),
(50, 'Tensione cella 17', 'In mV'),
(51, 'Tensione cella 18', 'In mV'),
(52, 'Tensione cella 19', 'In mV'),
(53, 'Tensione cella 20', 'In mV'),
(54, 'Tensione cella 21', 'In mV'),
(55, 'Tensione cella 22', 'In mV'),
(56, 'Tensione cella 23', 'In mV'),
(57, 'Tensione cella 24', 'In mV'),
(58, 'Tensione cella 25', 'In mV'),
(59, 'Tensione cella 26', 'In mV'),
(60, 'Tensione cella 27', 'In mV'),
(61, 'Tensione cella 28', 'In mV'),
(62, 'Tensione cella 29', 'In mV'),
(63, 'Tensione cella 30', 'In mV'),
(64, 'Tensione cella 31', 'In mV'),
(65, 'Temperatura 0 pacco batteria', 'In °C'),
(66, 'Temperatura 1 pacco batteria', 'In °C'),
(67, 'Temperatura BMS', 'In °C'),
(68, 'Corrente massima uscente', 'In mA'),
(69, 'Corrente massima entrante', 'In mA'),
(70, 'RPM motore', 'In RPM'),
(71, 'Corrente motore', 'In A'),
(72, 'Tensione controller motore', 'In V (Letta dal controller)'),
(73, 'Input accelleratore', '0 = 0%, 255 = 100%'),
(74, 'Temperatura controller', 'In °C'),
(75, 'Temperatura motore', 'In °C'),
(76, 'Corrente pacco batteria', 'In mA (Letta dallo shunt)'),
(77, 'Tensione batterie', 'In mV (Letta dallo shunt)'),
(78, 'Tensione servizi', 'In mV (Letta dallo shunt)'),
(79, 'Tensione controller motore', 'In mV (Letta dallo shunt)'),
(80, 'Temperatura shunt', ''),
(81, 'Potenza Shunt', 'In mW');

-- --------------------------------------------------------

--
-- Struttura della tabella `pacchetto`
--

CREATE TABLE `pacchetto` (
  `id` bigint(20) NOT NULL,
  `cod_legenda` int(11) NOT NULL,
  `valore` double NOT NULL,
  `timestamp` timestamp(6) NOT NULL DEFAULT current_timestamp(6) ON UPDATE current_timestamp(6),
  `cod_veicolo` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Struttura della tabella `veicolo`
--

CREATE TABLE `veicolo` (
  `id` int(11) NOT NULL,
  `nome` varchar(40) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dump dei dati per la tabella `veicolo`
--

INSERT INTO `veicolo` (`id`, `nome`) VALUES
(7, 'Lada'),
(14, 'Fiat 500 jolly '),
(16, 'Fiat 500'),
(44, 'Mini cooper'),
(46, 'Ape Piaggio'),
(64, 'Fiat Punto');

--
-- Indici per le tabelle scaricate
--

--
-- Indici per le tabelle `legenda`
--
ALTER TABLE `legenda`
  ADD PRIMARY KEY (`id`);

--
-- Indici per le tabelle `pacchetto`
--
ALTER TABLE `pacchetto`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_legenda` (`cod_legenda`),
  ADD KEY `fk_veicolo` (`cod_veicolo`);

--
-- Indici per le tabelle `veicolo`
--
ALTER TABLE `veicolo`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT per le tabelle scaricate
--

--
-- AUTO_INCREMENT per la tabella `legenda`
--
ALTER TABLE `legenda`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=82;

--
-- AUTO_INCREMENT per la tabella `pacchetto`
--
ALTER TABLE `pacchetto`
  MODIFY `id` bigint(20) NOT NULL AUTO_INCREMENT;

--
-- Limiti per le tabelle scaricate
--

--
-- Limiti per la tabella `pacchetto`
--
ALTER TABLE `pacchetto`
  ADD CONSTRAINT `fk_legenda` FOREIGN KEY (`cod_legenda`) REFERENCES `legenda` (`id`),
  ADD CONSTRAINT `fk_veicolo` FOREIGN KEY (`cod_veicolo`) REFERENCES `veicolo` (`id`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
