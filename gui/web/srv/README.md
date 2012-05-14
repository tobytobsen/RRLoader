# Info

This script starts the built-in PHP-CLI Webserver for the WebUI.
You need at least PHP 5.4 to run this script!

Just execute boot.php (it's a shell-script).

# For Windows Users

Execute boot.exe inside the `bin` folder.

Edit the PHP_CLI path in your "boot.ini" file if the application 
can't access the PHP-Interpreter.

This script works fine with WAMP/XAMPP PHP binaries.

If you don't have PHP installed on your system read the "Portable notice".

#### Portable notice

If you don't have PHP 5.4 installed yet, you can get a copy
at <http://windows.php.net/download/>.

Unpack the archive into the `bin/php` folder and set 
the PHP_CLI path inside the "boot.ini" file to "./bin/php/php.exe"

Example:

``` ini
[PHP_CLI]
path = "./bin/php/php.exe"
```

Thats it, you're done.
