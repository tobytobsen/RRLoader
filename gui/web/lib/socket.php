<?php

/**
 * Simple socket API for the RRLoader Daemon
 * Copyright 2012 - droptable/murdoc <murdoc@raidrush.org>
 */
 
namespace rrl;

use \Exception;
use \InvalidArgumentException;

class Socket 
{
  const DEFAULT_HOST = 'localhost',
        DEFAULT_PORT = 50100; // WIP
  
  // standard C errorcodes are at the bottom
  const ERR_MISSING = 10000;
        
  protected $sock, $conf;
  
  // @throws \InvalidArgumentException
  public function __construct(array $conf = [])
  {
    if (!function_exists('socket_create'))
      throw new Exception('missing socket support', self::ERR_MISSING);
      
    $conf += [ 'host' => self::DEFAULT_HOST, 'port' => self::DEFAULT_PORT ];
    
    if (!is_string($conf['host']) || empty($conf['host'])
     || !is_integer($conf['port']) || $conf['port'] <= 1023) {
      throw new InvalidArgumentException(
        '`host` must not be empty and `port` must be of type integer > 1023. '
        . 'if you don\'t know the correct values for these variables, '
        . 'try to use the built-in default values.'
      );
    }
    
    $this->conf = &$conf;
    
    // -------------------------
    
    $this->connect();
  }
  
  // ---------------------------
  // @public
  
  // @throws \Exception
  public function write($msg)
  {
    $r = [];
    $w = [ $this->sock ];
    $e = [];
    
    socket_select($r, $w, $e, 10);
      
    $len = strlen($msg);
    $res = socket_write($this->sock, $msg, $len);
    
    if ($res === false)
      $this->throw_socket_error();
    
    if ($res < $len) {
      // $res = gesendet
      // $len = länge
      $len -= $res;
      
      while ($len) {
        $msg = substr($msg, $res);
        $res = socket_write($this->sock, $msg, $len);
        
        if (false === $res)
          $this->throw_socket_error();
          
        $len -= $res;
      }
    }
    
    return $this;
  }
  
  // @throws \Exception
  public function read($bts = 0)
  {
    $r = [ $this->sock ];
    $w = [];
    $e = [];
    
    socket_select($r, $w, $e, 10);
    
    $buf = socket_read($this->sock, $bts ?: 4096, PHP_BINARY_READ);
    
    if (false === $buf)
      $this->throw_socket_error();
    
    $len = strlen($buf);
    
    if ($bts !== 0 && $len === $bts)
      return $buf;
      
    if ($bts === 0) {
      while (socket_select($r, $w, $e, 0) > 0) {
        $cnk = socket_read($this->sock, 4096, PHP_BINARY_READ);
        
        if (false === $cnk)
          $this->throw_socket_error();
        
        if ($cnk === '')
          return $buf;
          
        $buf .= $cnk;
      }
      
      return $buf;
    }
      
    while ($bts) {
      if (socket_select($r, $w, $e, 0) != 1)
        break;
      
      $bts -= $len;
      $cnk = socket_read($this->sock, $bts, PHP_BINARY_READ);
      
      if (false === $cnk)
        $this->throw_socket_error();
      
      if ($cnk === '')
        return $buf;
      
      $len += strlen($cnk);
      $buf .= $cnk;
    }
    
    return $buf;
  }
  
  // @throws Exception
  public function close()
  {
    socket_clear_error($this->sock);
    socket_close($this->sock);
  }
  
  // ---------------------------
  // @protected
  
  // @throws Exception
  protected function throw_socket_error($errno = -1)
  {
    if ($errno === -1)
      $errno = socket_last_error($this->sock);
      
    throw new Exception(socket_strerror($errno), $errno);
  }
  
  // @throws \Exception
  protected function connect()
  {
    $this->sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    
    if (false === $this->sock)
      $this->throw_socket_error();
      
    // this is a service, not a server ;)
    socket_set_block($this->sock);
    socket_set_option($this->sock, SOL_SOCKET, SO_RCVTIMEO, [ 'sec' => 1, 'usec' => 500000]);
    
    if (!socket_connect($this->sock, $this->conf['host'], $this->conf['port']))
      $this->throw_socket_error();
  }
  
  // -------------------------
  
  const ERR_NOTSOCK         = 88,    /* Socket operation on non-socket */
        ERR_DESTADDRREQ     = 89,    /* Destination address required */
        ERR_MSGSIZE         = 90,    /* Message too long */
        ERR_PROTOTYPE       = 91,    /* Protocol wrong type for socket */
        ERR_NOPROTOOPT      = 92,    /* Protocol not available */
        ERR_PROTONOSUPPORT  = 93,    /* Protocol not supported */
        ERR_SOCKTNOSUPPORT  = 94,    /* Socket type not supported */
        ERR_OPNOTSUPP       = 95,    /* Operation not supported on transport endpoint */
        ERR_PFNOSUPPORT     = 96,    /* Protocol family not supported */
        ERR_AFNOSUPPORT     = 97,    /* Address family not supported by protocol */
        ERR_ADDRINUSE       = 98,    /* Address already in use */
        ERR_ADDRNOTAVAIL    = 99,    /* Cannot assign requested address */
        ERR_NETDOWN         = 100,   /* Network is down */
        ERR_NETUNREACH      = 101,   /* Network is unreachable */
        ERR_NETRESET        = 102,   /* Network dropped connection because of reset */
        ERR_CONNABORTED     = 103,   /* Software caused connection abort */
        ERR_CONNRESET       = 104,   /* Connection reset by peer */
        ERR_NOBUFS          = 105,   /* No buffer space available */
        ERR_ISCONN          = 106,   /* Transport endpoint is already connected */
        ERR_NOTCONN         = 107,   /* Transport endpoint is not connected */
        ERR_SHUTDOWN        = 108,   /* Cannot send after transport endpoint shutdown */
        ERR_TOOMANYREFS     = 109,   /* Too many references: cannot splice */
        ERR_TIMEDOUT        = 110,   /* Connection timed out */
        ERR_CONNREFUSED     = 111,   /* Connection refused */
        ERR_HOSTDOWN        = 112,   /* Host is down */
        ERR_HOSTUNREACH     = 113,   /* No route to host */
        ERR_ALREADY         = 114,   /* Operation already in progress */
        ERR_INPROGRESS      = 115,   /* Operation now in progress */
        ERR_REMOTEIO        = 121,   /* Remote I/O error */
        ERR_CANCELED        = 125;   /* Operation Canceled */ 
}
