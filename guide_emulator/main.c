#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <gtk/gtk.h>

static const char* portname = "/dev/ttyUSB0";

GtkWidget *ra_plus_pulse_entry;
GtkWidget *ra_minus_pulse_entry;
GtkWidget *dec_plus_pulse_entry;
GtkWidget *dec_minus_pulse_entry;

void show_error(const int err, const char* port, gpointer window) {
    
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Failed to open %s \nError: %s", port, strerror(err));
  gtk_window_set_title(GTK_WINDOW(dialog), "Error");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void rp_callback(GtkWidget *widget, gpointer data)
{
  g_print ("ra+ was pressed\n");

  int fd = *(int*)data;

  unsigned char cmd = 0xF2;

  write(fd, &cmd, sizeof(cmd));

  sleep(5);

  cmd = 0xF3;

  write(fd, &cmd, sizeof(cmd));
}

void rm_callback(GtkWidget *widget, gpointer data)
{
  g_print ("ra- was pressed\n");

  int fd = *(int*)data;

  unsigned char cmd = 0xF4;

  write(fd, &cmd, sizeof(cmd));

  sleep(5);

  cmd = 0xF5;

  write(fd, &cmd, sizeof(cmd));
}

void dp_callback(GtkWidget *widget, gpointer data)
{
  g_print ("dec+ was pressed\n");

  int fd = *(int*)data;

  unsigned char cmd = 0xF8;

  write(fd, &cmd, sizeof(cmd));

  sleep(5);

  cmd = 0xF9;

  write(fd, &cmd, sizeof(cmd));
}

void dm_callback(GtkWidget *widget, gpointer data)
{
  g_print ("dec- was pressed\n");

  int fd = *(int*)data;

  unsigned char cmd = 0xFA;

  write(fd, &cmd, sizeof(cmd));

  sleep(5);

  cmd = 0xFB;

  write(fd, &cmd, sizeof(cmd));
}

void rpp_callback(GtkWidget *widget, gpointer data)
{
  const char* entry_txt_val = gtk_entry_get_text(ra_plus_pulse_entry);

  g_print ("ra+ pulse was pressed\n");
  g_print ("generating pulse for %s seconds\n", entry_txt_val);

  int fd = *(int*)data;

  unsigned char cmd = 0xC2;

  write(fd, &cmd, sizeof(cmd));

  cmd = strtol(entry_txt_val, NULL, 10);

  write(fd, &cmd, sizeof(cmd));
}

void rmp_callback(GtkWidget *widget, gpointer data)
{
  const char* entry_txt_val = gtk_entry_get_text(ra_minus_pulse_entry);

  g_print ("ra- pulse was pressed\n");
  g_print ("generating pulse for %s seconds\n", entry_txt_val);

  int fd = *(int*)data;

  unsigned char cmd = 0xC3;

  write(fd, &cmd, sizeof(cmd));

  cmd = strtol(entry_txt_val, NULL, 10);

  write(fd, &cmd, sizeof(cmd));
}

void dpp_callback(GtkWidget *widget, gpointer data)
{
  const char* entry_txt_val = gtk_entry_get_text(dec_plus_pulse_entry);

  g_print ("dec+ pulse was pressed\n");
  g_print ("generating pulse for %s seconds\n", entry_txt_val);

  int fd = *(int*)data;

  unsigned char cmd = 0xC4;

  write(fd, &cmd, sizeof(cmd));

  cmd = strtol(entry_txt_val, NULL, 10);

  write(fd, &cmd, sizeof(cmd));
}

void dmp_callback(GtkWidget *widget, gpointer data)
{
  const char* entry_txt_val = gtk_entry_get_text(dec_minus_pulse_entry);

  g_print ("dec- pulse was pressed\n");
  g_print ("generating pulse for %s seconds\n", entry_txt_val);

  int fd = *(int*)data;

  unsigned char cmd = 0xC5;

  write(fd, &cmd, sizeof(cmd));

  cmd = strtol(entry_txt_val, NULL, 10);

  write(fd, &cmd, sizeof(cmd));
}

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //error_message ("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        //if (tcsetattr (fd, TCSANOW, &tty) != 0)
                //error_message ("error %d setting term attributes", errno);
}

int open_serial()
{
  int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);

  if (fd < 0) {
    return -1;
  }

  set_interface_attribs(fd, B9600, 0);
  set_blocking (fd, 0);

  return fd;
}

int main(int argc, char *argv[]) {

  GtkWidget *window;
  GtkWidget *ra_plus_button;
  GtkWidget *ra_minus_button;
  GtkWidget *dec_plus_button;
  GtkWidget *dec_minus_button;
  GtkWidget *ra_plus_pulse_button;
  GtkWidget *ra_minus_pulse_button;
  GtkWidget *dec_plus_pulse_button;
  GtkWidget *dec_minus_pulse_button;

  GtkWidget *sec_text_label1;
  GtkWidget *sec_text_label2;
  GtkWidget *sec_text_label3;
  GtkWidget *sec_text_label4;

  GtkWidget *halign;

  gtk_init(&argc, &argv);

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window), 230, 270);
  gtk_window_set_title(GTK_WINDOW(window), "Эмулятор гида");
  gtk_window_set_icon_from_file(GTK_WINDOW(window), "search-find-telescope-icon.png", NULL);

  int serial_fd = open_serial();

  if (serial_fd < 0) {
      show_error(errno, portname, G_OBJECT(window));
      exit(1);
  }

  halign = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), halign);

  ra_plus_button = gtk_button_new_with_label("RA +");
  gtk_signal_connect(GTK_OBJECT(ra_plus_button), "clicked", GTK_SIGNAL_FUNC(rp_callback), &serial_fd);

  ra_minus_button = gtk_button_new_with_label("RA -");
  gtk_signal_connect(GTK_OBJECT(ra_minus_button), "clicked", GTK_SIGNAL_FUNC(rm_callback), &serial_fd);

  dec_plus_button = gtk_button_new_with_label("DEC +");
  gtk_signal_connect(GTK_OBJECT(dec_plus_button), "clicked", GTK_SIGNAL_FUNC(dp_callback), &serial_fd);

  dec_minus_button = gtk_button_new_with_label("DEC -");
  gtk_signal_connect(GTK_OBJECT(dec_minus_button), "clicked", GTK_SIGNAL_FUNC(dm_callback), &serial_fd);

  ra_plus_pulse_button = gtk_button_new_with_label("RA + pulse");
  gtk_signal_connect(GTK_OBJECT(ra_plus_pulse_button), "clicked", GTK_SIGNAL_FUNC(rpp_callback), &serial_fd);

  ra_minus_pulse_button = gtk_button_new_with_label("RA - pulse");
  gtk_signal_connect(GTK_OBJECT(ra_minus_pulse_button), "clicked", GTK_SIGNAL_FUNC(rmp_callback), &serial_fd);

  dec_plus_pulse_button = gtk_button_new_with_label("DEC + pulse");
  gtk_signal_connect(GTK_OBJECT(dec_plus_pulse_button), "clicked", GTK_SIGNAL_FUNC(dpp_callback), &serial_fd);

  dec_minus_pulse_button = gtk_button_new_with_label("DEC - pulse");
  gtk_signal_connect(GTK_OBJECT(dec_minus_pulse_button), "clicked", GTK_SIGNAL_FUNC(dmp_callback), &serial_fd);

  gtk_fixed_put(GTK_FIXED(halign), ra_plus_button, 10, 10);
  gtk_widget_set_size_request(ra_plus_button, 80, 30);

  gtk_fixed_put(GTK_FIXED(halign), ra_minus_button, 10, 50);
  gtk_widget_set_size_request(ra_minus_button, 80, 30);

  gtk_fixed_put(GTK_FIXED(halign), dec_plus_button, 100, 10);
  gtk_widget_set_size_request(dec_plus_button, 80, 30);

  gtk_fixed_put(GTK_FIXED(halign), dec_minus_button, 100, 50);
  gtk_widget_set_size_request(dec_minus_button, 80, 30);


  gtk_fixed_put(GTK_FIXED(halign), ra_plus_pulse_button, 10, 100);
  gtk_widget_set_size_request(ra_plus_pulse_button, 87, 30);

  gtk_fixed_put(GTK_FIXED(halign), ra_minus_pulse_button, 10, 140);
  gtk_widget_set_size_request(ra_minus_pulse_button, 87, 30);

  gtk_fixed_put(GTK_FIXED(halign), dec_plus_pulse_button, 10, 185);
  gtk_widget_set_size_request(dec_plus_pulse_button, 87, 30);

  gtk_fixed_put(GTK_FIXED(halign), dec_minus_pulse_button, 10, 225);
  gtk_widget_set_size_request(dec_minus_pulse_button, 87, 30);


  ra_plus_pulse_entry = gtk_entry_new();
  ra_minus_pulse_entry = gtk_entry_new();
  dec_plus_pulse_entry = gtk_entry_new();
  dec_minus_pulse_entry = gtk_entry_new();

  sec_text_label1 = gtk_label_new("sec");
  sec_text_label2 = gtk_label_new("sec");
  sec_text_label3 = gtk_label_new("sec");
  sec_text_label4 = gtk_label_new("sec");

  gtk_fixed_put(GTK_FIXED(halign), ra_plus_pulse_entry, 107, 100);
  gtk_widget_set_size_request(ra_plus_pulse_entry, 80, 30);

  gtk_fixed_put(GTK_FIXED(halign), ra_minus_pulse_entry, 107, 140);
  gtk_widget_set_size_request(ra_minus_pulse_entry, 80, 30);

  gtk_fixed_put(GTK_FIXED(halign), dec_plus_pulse_entry, 107, 185);
  gtk_widget_set_size_request(dec_plus_pulse_entry, 80, 30);

  gtk_fixed_put(GTK_FIXED(halign), dec_minus_pulse_entry, 107, 225);
  gtk_widget_set_size_request(dec_minus_pulse_entry, 80, 30);

  gtk_entry_set_text((GtkEntry*) ra_plus_pulse_entry, "1");
  gtk_entry_set_text((GtkEntry*) ra_minus_pulse_entry, "1");
  gtk_entry_set_text((GtkEntry*) dec_plus_pulse_entry, "1");
  gtk_entry_set_text((GtkEntry*) dec_minus_pulse_entry, "1");

  gtk_fixed_put(GTK_FIXED(halign), sec_text_label1, 190, 110);
  gtk_fixed_put(GTK_FIXED(halign), sec_text_label2, 190, 150);
  gtk_fixed_put(GTK_FIXED(halign), sec_text_label3, 190, 190);
  gtk_fixed_put(GTK_FIXED(halign), sec_text_label4, 190, 235);

  gtk_widget_show_all(window);
  
  g_signal_connect(window, "destroy",
  G_CALLBACK(gtk_main_quit), NULL);  

  gtk_main();

  close(serial_fd);

  return 0;
}

