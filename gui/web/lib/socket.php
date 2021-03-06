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
  const DEFAULT_HOST = 'localhost', // WIP
        DEFAULT_PORT = 50100;       // WIP
  
  // standard C errorcodes are at the bottom of this file
  
  const ERR_MISSING = 10000, // missing sockets extension
        ERR_CREATE  = 10001; // socket could not be created
        
  protected $sock, $conf;
  
  /**
   * constructor
   *
   * @param   array     $conf   [optional]
   * @throws \InvalidArgumentException
   */
  public function __construct(array $conf = [])
  {
    if (!extension_loaded('sockets'))
      throw new Exception('missing sockets extension', self::ERR_MISSING);
      
    $conf += [ 'host' => self::DEFAULT_HOST, 'port' => self::DEFAULT_PORT ];
    
    if (!is_string($conf['host']) || empty($conf['host']) || !is_int($conf['port'])) {
      throw new InvalidArgumentException(
        '`host` must not be empty and `port` must be of type integer. '
        . 'if you don\'t know the correct values for these variables, '
        . 'try to use the built-in default values.'
      );
    }
    
    $this->conf = &$conf;
    $this->connect();
  }
  
  // ---------------------------
  // @public
  
  /**
   * writes data
   *
   * @param   string    $msg
   * @return  self
   * @throws \Exception
   */
  public function write($msg)
  {
    $r = [];
    $w = [ $this->sock ];
    $e = [];
    
    socket_select($r, $w, $e, 10);
      
    $len = strlen($msg);
    $res = socket_write($this->sock, $msg, $len);
    
    if ($res === false)
      $this->throwSocketError();
    
    if ($res < $len) {
      $len -= $res;
      
      while ($len) {
        $msg = substr($msg, $res);
        $res = socket_write($this->sock, $msg, $len);
        
        if (false === $res)
          $this->throwSocketError();
          
        $len -= $res;
      }
    }
    
    return $this;
  }
  
  /**
   * reads data
   *
   * note:
   * if $bts is 0 this method reads all available data in the buffer
   *
   * note:
   * $bts can be greater than the amount of available data.
   * this method will stop reading if noting more is available.
   *
   * if your application requires a specific amount of data use `recv`
   *
   * @param   int       $bts
   * @return string
   * @throws \Exception
   */
  public function read($bts = 0)
  {
    $r = [ $this->sock ];
    $w = [];
    $e = [];
    
    socket_select($r, $w, $e, 10);
    
    $buf = socket_read($this->sock, $bts ?: 4096, PHP_BINARY_READ);
    
    if (false === $buf)
      $this->throwSocketError();
    
    $len = strlen($buf);
    
    if ($bts !== 0 && $len === $bts)
      return $buf;
      
    if ($bts === 0) {
      while (socket_select($r, $w, $e, 0) > 0) {
        $cnk = socket_read($this->sock, 4096, PHP_BINARY_READ);
        
        if (false === $cnk)
          $this->throwSocketError();
          
        if ($cnk === '')
          return $buf;
        
        $buf .= $cnk;
      }
      
      return $buf;
    }
    
    $bts -= $len;
    
    while ($bts) {
      if (socket_select($r, $w, $e, 0) != 1)
        break;
        
      $cnk = socket_read($this->sock, $bts, PHP_BINARY_READ);
      
      if (false === $cnk)
        $this->throwSocketError();
      
      if ($cnk === '')
        return $buf;
      
      $len = strlen($cnk);
      $bts -= $len;
      $buf .= $cnk;
    }
    
    return $buf;
  }
  
  /** 
   * reads a specific amount of bytes
   *
   * note:
   * this method blocks until the requested amount of bytes
   * could be read from the socket!
   *
   * if you just want the current buffer use `read` instead
   *
   * note:
   * this method works in two ways:
   *
   * 1. if you use a format, it will unpack the data for you.
   * 2. if you don't use a format, it will return the data as string.
   *
   * note: you can set $bts to a format if the size is known.
   * supported sizes are: c,C,x,s,S,n,v,l,L,N,V (see ::sizeof())
   *
   * @param   int     $bts
   * @param   string  $fmt
   * @return mixed
   * @throws \Exception
   */
  public function recv($bts, $fmt = '')
  {
    $r = [ $this->sock ];
    $w = [];
    $e = [];
    
    socket_select($r, $w, $e, 10);
    
    if (is_string($bts))
      $bts = $this->sizeof($fmt = $bts);
      
    $buf = '';
    $res = socket_recv($this->sock, $buf, $bts, MSG_WAITALL);
    
    if ($res === false)
      $this->throwSocketError();
    
    if (empty($fmt)) return $buf;
    
    $buf = unpack($fmt, $buf);
    return (count($buf) > 1) ? $buf : ($buf[1] ?: null);
  }
  
  /**
   * closes the socket
   *
   * @void
   */
  public function close()
  {
    socket_clear_error($this->sock);
    socket_close($this->sock);
  }
  
  // ---------------------------
  // @protected
  
  /**
   * returns the size of a pack-flag in bytes
   *
   * @param   string    $fmt
   * @return  int
   */
  protected function sizeof($fmt)
  {
    // see <http://www.php.net/manual/de/function.pack.php> 
    switch ($fmt) {
      case 'c': case 'C': case 'x':
        return 1;
        
      case 's': case 'S': case 'n': case 'v':
        return 2;
        
      case 'l': case 'L': case 'N': case 'V':
        return 4;
        
      default:            
        throw new InvalidArgumentException(
          'the requested format "' . $fmt . '" does not have '
          . 'a fixed width. please provide the requested amount '
          . 'of bytes if you want to use a format like that! '
          . 'you can also omit the format-parameter and unpack '
          . 'the raw-data by yourself.'
        );
    }
  } 
  
  /**
   * throws an socket-error exception
   * 
   * @param   int     $errno    [optional]
   * @throws \Exception
   */
  protected function throwSocketError($errno = -1)
  {
    if ($errno === -1)
      $errno = socket_last_error($this->sock);
      
    throw new Exception(socket_strerror($errno), $errno);
  }
  
  /**
   * creates a socket and trys to connect to the given host/port
   * 
   * @throws \Exception
   */
  protected function connect()
  {
    $this->sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    
    if (false === $this->sock)
      throw new Exception('socket could not be created', self::ERR_CREATE);
      
    // this is a service, not a server ;)
    socket_set_block($this->sock);
    socket_set_option($this->sock, SOL_SOCKET, SO_RCVTIMEO, [ 'sec' => 1, 'usec' => 500000]);
    
    if (!socket_connect($this->sock, $this->conf['host'], $this->conf['port']))
      $this->throwSocketError();
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
