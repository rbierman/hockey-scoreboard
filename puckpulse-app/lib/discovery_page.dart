import 'package:bonsoir/bonsoir.dart';
import 'package:flutter/material.dart';
import 'discovery_service.dart';
import 'scoreboard_model.dart';

class DiscoveryPage extends StatelessWidget {
  final DiscoveryService discoveryService;
  final Function(ScoreboardInstance) onServiceSelected;

  const DiscoveryPage({
    super.key,
    required this.discoveryService,
    required this.onServiceSelected,
  });

  @override
  Widget build(BuildContext context) {
    return ListenableBuilder(
      listenable: discoveryService,
      builder: (context, _) {
        if (!discoveryService.isDiscoverySupported) {
          return _buildErrorState(discoveryService.discoveryError);
        }

        final services = discoveryService.discoveredServices;

        if (services.isEmpty) {
          return _buildEmptyState();
        }

        return _buildDiscoveryList(services);
      },
    );
  }

  Widget _buildErrorState(String error) {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          const Icon(Icons.error_outline, size: 64, color: Colors.red),
          const SizedBox(height: 16),
          Text('Discovery Error: $error'),
        ],
      ),
    );
  }

  Widget _buildEmptyState() {
    return const Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          CircularProgressIndicator(),
          SizedBox(height: 24),
          Text('Searching for scoreboards...', style: TextStyle(fontSize: 18)),
          SizedBox(height: 8),
          Text(
            'Ensure the puckpulse-controller is running on the same network.',
            style: TextStyle(color: Colors.grey, fontSize: 12),
          ),
        ],
      ),
    );
  }

  Widget _buildDiscoveryList(List<BonsoirService> services) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Padding(
          padding: EdgeInsets.all(16.0),
          child: Text(
            'Select a Scoreboard:',
            style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold),
          ),
        ),
        Expanded(
          child: ListView.builder(
            itemCount: services.length,
            itemBuilder: (context, index) {
              final service = services[index];
              final version = service.attributes['version'];
              return ListTile(
                leading: const Icon(Icons.sports_hockey, color: Colors.blue),
                title: Row(
                  children: [
                    Expanded(child: Text(service.name)),
                    if (version != null)
                      Text(
                        'v$version',
                        style: Theme.of(context).textTheme.bodySmall?.copyWith(
                          color: Colors.grey,
                        ),
                      ),
                  ],
                ),
                subtitle: Text(service.toJson()['service.host'] ?? 'Resolving...'),
                trailing: const Icon(Icons.chevron_right),
                onTap: () {
                  final host = service.toJson()['service.host'];
                  if (host != null && host != '0.0.0.0') {
                    onServiceSelected(ScoreboardInstance(
                      name: service.name,
                      host: host,
                      port: service.port,
                      version: version,
                    ));
                  }
                },
              );
            },
          ),
        ),
      ],
    );
  }
}
