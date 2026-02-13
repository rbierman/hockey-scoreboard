import 'dart:convert';
import 'dart:async';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'scoreboard_state.dart';

class WebSocketService {
  WebSocketChannel? _channel;
  final _stateController = StreamController<ScoreboardState>.broadcast();

  Stream<ScoreboardState> get stateStream => _stateController.stream;

  Future<void> connect(String host, int port) async {
    final uri = Uri.parse('ws://$host:$port');
    _channel = WebSocketChannel.connect(uri);

    _channel!.stream.listen((message) {
      try {
        final Map<String, dynamic> data = jsonDecode(message);
        final state = ScoreboardState.fromJson(data);
        _stateController.add(state);
      } catch (e) {
        print('Error parsing scoreboard state: $e');
      }
    }, onDone: () {
      print('WebSocket connection closed');
    }, onError: (error) {
      print('WebSocket error: $error');
    });
  }

  void sendCommand(String command, {dynamic value, int? delta, int? index, int? player}) {
    if (_channel == null) return;
    
    final Map<String, dynamic> payload = {'command': command};
    if (value != null) payload['value'] = value;
    if (delta != null) payload['delta'] = delta;
    if (index != null) payload['index'] = index;
    if (player != null) payload['player'] = player;

    _channel!.sink.add(jsonEncode(payload));
  }

  void dispose() {
    _channel?.sink.close();
    _stateController.close();
  }
}
