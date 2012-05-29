// PROTOTYPE

var RRL;
if (!RRL) RRL = {};

RRL.WebSocket = (function(win, doc) {
  var WebSocket = win.WebSocket
    || win.MozWebSocket
    || win.WebkitWebSocket
    || null;
  
  // UGLY BROWSER SNIFFING
  // FF 11+ (Seamonkey 2.8), Chrome 16+, IE 10, Webkit 533.17.9+, maybe Opera 12?
  var SUPPORTED = WebSocket && (function() {
    var ua = win.navigator.userAgent.toString(),
        vn = ua.match(/(firefox|seamonkey|chrome|msie|webkit)\/(\d+(?:\.\d+)*)/i);
        
    if (!vn) return false;
        
    switch (vn[1].toLowerCase()) {
      case 'firefox':
        return parseFloat(vn[2]) >= 11;
        
      case 'chrome':
        return parseFloat(vn[2]) >= 16;
        
      case 'webkit':
        var sum = 0;
        
        vn[2].split('.').reverse().forEach(function(n, i) {
          sum += n * Math.pow(100, i);
        });
        
        // 533.17.9
        return sum >= 5331709;
        
      case 'msie':
        return parseFloat(vn[2]) >= 10;
        
      case 'seamonkey':
        return parseFloat(vn[2]) >= 2.8;
    }
    
    return false;
  })();
  
  if (!SUPPORTED)
    throw new Error('your browser does not support WebSockets or is using an outdated version');
  
  return WebSocket;
})(window);

