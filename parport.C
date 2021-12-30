
#include <parport.H>

int parport_driver::open()
{
 last_errno = 0;
 fd = ::open(device, O_RDWR);
 if (fd < 0) {
  last_errno = errno;
  return -1;
 }
 if ((ioctl(fd, PPEXCL) == -1) || (ioctl(fd, PPCLAIM) == -1)) {
  last_errno = errno;
  ioctl(fd, PPRELEASE);
  ::close(fd);
  return -1;
 }
 dev_status = OPENED_STATE;
 return 0;
}

int parport_driver::close()
{
 last_errno = 0;
 if (ioctl(fd, PPRELEASE) == -1) {
  last_errno = errno;
  ::close(fd);
  fd = -1;
  dev_status = CLOSED_STATE;
  return -1;
 }
 int res = ::close(fd);
 fd = -1;
 if (res)
  last_errno = errno;
 dev_status = CLOSED_STATE;
 return res;
}

int parport_driver::set_data(int data)
{
 last_errno = 0;
 if (ioctl(fd, PPWDATA, &data) == -1) {
  last_errno = errno;
  return -1;
 }
 return 0;
}

int parport_driver::get_data(unsigned char &data)
{
 last_errno = 0;
 if (ioctl(fd, PPRDATA, &data) == -1) {
  last_errno = errno;
  return -1;
 }
 return 0;
}

int parport_driver::status(unsigned char &status)
{
 last_errno = 0;
 if (ioctl(fd, PPRSTATUS, &status) == -1) {
  last_errno = errno;
  return -1;
 }
 status ^= 0x80; // BUSY is inverted
 return 0;
}

int parport_driver::control(unsigned char data)
{
 last_errno = 0;
 data ^= 0x0B; // SELECT, AUTOFD, and STROBE are inverted
 if (ioctl(fd, PPWCONTROL, &data) == -1) {
  last_errno = errno;
  return -1;
 }
 return 0;
}
