"""Python module that implements the loader protocol over a serial port.
"""
import serial
import time
import sys

class LoaderException(Exception):
    pass

class Loader:
    """Host side implementation of the serial loader protocol.  Use to
    load programs into the device and start execution.
    """
    def __init__(self, port, rate=115200):
        self._s = serial.Serial(port, rate, timeout=0.010)
        
    def encode(self, msg):
        """Encode a message"""
        return '%s\r' % msg

    def send(self, msg):
        """Encode and send a message"""
        enc = self.encode(msg)
        self._s.write(enc)

    def get_nibble(self, ch):
        """Convert a nibble to an integer"""
        return '0123456789abcdef'.index(ch)

    def get(self):
        """Get and return the response, throwing on no response."""
        msg = ''
        end = time.time() + 1

        while True:
            got = self._s.read(4096)

            for ch in got:
                if ch == '\n':
                    pass
                elif ch == '\r':
                    return msg
                else:
                    msg += ch
        
            if time.time() > end:
                raise LoaderException('No response')

    def check_ok(self):
        """Check that the loader responded with OK"""
        got = self.get()

        if got != 'k':
            raise LoaderException('Target returned "%s"' % got)

    def read(self, address, count):
        """Read count bytes starting at address and return as a string"""
        self.send('m%x %x' % (address, count))
        return self.get()

    def write(self, address, data):
        """Write the given data to the given address"""
        payload = ''.join('%02x' % ord(ch) for ch in data)
        self.send('M%x %x %s' % (address, len(data), payload))
        self.check_ok()

    def writebin(self, address, data):
        """Write the given data to the given address in binary transfer mode"""
        special = '\r\n\003}'
        payload = ''.join('}%s' % chr(ord(x) ^ 0x20) if x in special else x for x in data)
        self.send('X%x %s' % (address, payload))
        self.check_ok()

    def checksum(self, address, count):
        """Get the sixteen bit checksum of count bytes starting at the
        given address
        """
        self.send('u%x %x' % (address, count))
        return int(self.get(), 16)

    def go(self, address):
        """Start execution at the given address"""
        self.send('c%x' % address)

        try:
            self.check_ok()
        except:
            # Ignore the exception after starting the program
            pass

    def set_baud(self, bps):
        """Change the baud rate"""
        fbrclk = 48000000
        div = 1
        divmd = 8
        brr = (((((10 * fbrclk) / div / divmd)/(2 * bps)) + 5) / 10 - 1)
        self.send('B%x' % brr)

    def load(self, address, bin):
        """Load the data at the given starting address.  Chunks the
        data up and verifies the checksum.

        Throws on any error.
        """
        chunk = 256

        for i in range(0, len(bin), chunk):
            self.writebin(address + i, bin[i:i+chunk])
            self.progress(address + i, 100.0 * i / len(bin))

        got = self.checksum(0x10040000, len(bin))
        expect = sum(ord(x) for x in bin) & 0xFFFF
    
        if got != expect:
            raise LoaderException('Checksum error')

    def interrupt(self):
        """Send an interrupt to the loader"""
        self._s.write('\003')

    def reset(self):
        """Reset the target"""
        self._s.setDTR(False)
        time.sleep(0.5)

        end = time.time() + 6

        while time.time() < end:
            self.interrupt()
            if self._s.read() == '!':
                break
        else:
            raise LoaderException('No response after reset')

        self._s.read(4096)
        self.interrupt()
        self.get()

    def progress(self, address, percent):
        """Prints out the progress.  Override for other versions."""
        print '%08x (%.1f %%)\r' % (address, percent)
        sys.stdout.flush()

def main():
    l = Loader('/dev/ttyUSB0')
    dat = open(sys.argv[1], 'rb').read()
    l.reset()
    l.load(0x10040000, dat)
    l.go(0x10040000)

if __name__ == '__main__':
    main()

