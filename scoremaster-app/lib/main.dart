import 'package:flutter/material.dart';
import 'discovery_page.dart';
import 'discovery_service.dart';
import 'scoreboard_control_page.dart';

void main() {
  runApp(const ScoreboardControlApp());
}

class ScoreboardControlApp extends StatefulWidget {
  const ScoreboardControlApp({super.key});

  @override
  State<ScoreboardControlApp> createState() => _ScoreboardControlAppState();
}

class _ScoreboardControlAppState extends State<ScoreboardControlApp> {
  final DiscoveryService _discoveryService = DiscoveryService();

  @override
  void initState() {
    super.initState();
    _discoveryService.start();
  }

  @override
  void dispose() {
    _discoveryService.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ScoreMaster',
      theme: ThemeData(
        brightness: Brightness.dark,
        primarySwatch: Colors.blue,
        useMaterial3: true,
      ),
      home: Scaffold(
        appBar: AppBar(title: const Text('ScoreMaster')),
        body: Builder(
          builder: (context) => DiscoveryPage(
            discoveryService: _discoveryService,
            onServiceSelected: (instance) {
              Navigator.push(
                context,
                MaterialPageRoute(
                  builder: (context) => ScoreboardControlPage(instance: instance),
                ),
              );
            },
          ),
        ),
      ),
    );
  }
}
