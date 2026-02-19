class ScoreboardInstance {
  final String name;
  final String host;
  final int port;
  final String? version;

  ScoreboardInstance({
    required this.name,
    required this.host,
    required this.port,
    this.version,
  });

  @override
  String toString() => '$name ($host:$port)';
}
