import 'dart:async';
import 'package:bonsoir/bonsoir.dart';
import 'package:flutter/foundation.dart';

class DiscoveryService extends ChangeNotifier {
  BonsoirDiscovery? _discovery;
  StreamSubscription<BonsoirDiscoveryEvent>? _subscription;
  final List<BonsoirService> _discoveredServices = [];
  bool _isDiscoverySupported = true;
  String _discoveryError = '';

  List<BonsoirService> get discoveredServices => List.unmodifiable(_discoveredServices);
  bool get isDiscoverySupported => _isDiscoverySupported;
  String get discoveryError => _discoveryError;

  Future<void> start() async {
    if (_discovery != null) return;

    try {
      _discovery = BonsoirDiscovery(type: '_hockey-score._tcp');
      await _discovery!.initialize();
      
      final eventStream = _discovery!.eventStream;
      if (eventStream != null) {
        _subscription = eventStream.listen(_onDiscoveryEvent);
        await _discovery!.start();
      } else {
        _isDiscoverySupported = false;
        _discoveryError = 'Discovery event stream is null after initialization.';
        notifyListeners();
      }
    } catch (e) {
      _isDiscoverySupported = false;
      _discoveryError = e.toString();
      notifyListeners();
    }
  }

  void _onDiscoveryEvent(BonsoirDiscoveryEvent event) {
    final service = event.service;
    if (service == null) return;

    switch (event) {
      case BonsoirDiscoveryServiceFoundEvent():
        if (!_discoveredServices.any((s) => s.name == service.name)) {
          _discoveredServices.add(service);
          notifyListeners();
        }
        service.resolve(_discovery!.serviceResolver);
        break;
      case BonsoirDiscoveryServiceResolvedEvent():
        int index = _discoveredServices.indexWhere((s) => s.name == service.name);
        if (index != -1) {
          _discoveredServices[index] = service;
          notifyListeners();
        }
        break;
      case BonsoirDiscoveryServiceLostEvent():
        _discoveredServices.removeWhere((s) => s.name == service.name);
        notifyListeners();
        break;
      default:
        break;
    }
  }

  Future<void> stop() async {
    await _subscription?.cancel();
    await _discovery?.stop();
    _subscription = null;
    _discovery = null;
    _discoveredServices.clear();
    notifyListeners();
  }

  @override
  void dispose() {
    stop();
    super.dispose();
  }
}
