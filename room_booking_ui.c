
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#define MAX_ROOMS 5

typedef struct {
    int room_number;
    int is_booked;
} Room;

Room rooms[MAX_ROOMS] = {
    {101, 0}, {102, 0}, {103, 0}, {104, 0}, {105, 0}
};

// Callback to handle booking
void on_book_button_clicked(GtkWidget *button, gpointer data) {
    GtkWidget **widgets = (GtkWidget **)data;
    GtkComboBoxText *room_dropdown = GTK_COMBO_BOX_TEXT(widgets[0]);
    GtkEntry *name_entry = GTK_ENTRY(widgets[1]);
    GtkEntry *phone_entry = GTK_ENTRY(widgets[2]);

    const char *room_text = gtk_combo_box_text_get_active_text(room_dropdown);
    const char *name = gtk_entry_get_text(name_entry);
    const char *phone = gtk_entry_get_text(phone_entry);

    GtkWidget *dialog;

    if (!room_text || !*name || !*phone) {
        dialog = gtk_message_dialog_new(NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Please fill in all fields.");
    } else {
        int room_number = atoi(room_text);
        for (int i = 0; i < MAX_ROOMS; i++) {
            if (rooms[i].room_number == room_number) {
                if (rooms[i].is_booked) {
                    dialog = gtk_message_dialog_new(NULL,
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Room %d is already booked!", room_number);
                } else {
                    rooms[i].is_booked = 1;
                    dialog = gtk_message_dialog_new(NULL,
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_INFO,
                        GTK_BUTTONS_OK,
                        "Room %d successfully booked!\nName: %s\nPhone: %s",
                        room_number, name, phone);
                }
                break;
            }
        }
    }

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Activate callback
void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *logo;
    GtkWidget *room_label, *name_label, *phone_label;
    GtkWidget *room_dropdown, *name_entry, *phone_entry;
    GtkWidget *book_button;
    GtkWidget *widgets[3];

    // Create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Room Booking Service");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    // Create a grid layout
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Add logo
GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("logo.png", NULL);
if (pixbuf) {
    GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 100, 100, GDK_INTERP_BILINEAR); // Adjust size here
    logo = gtk_image_new_from_pixbuf(scaled_pixbuf);
    g_object_unref(pixbuf);       // Free original pixbuf
    g_object_unref(scaled_pixbuf); // Free scaled pixbuf after use
} else {
    g_warning("Failed to load logo.png. Please ensure the file exists.");
    logo = gtk_label_new("Logo Missing");
}
gtk_grid_attach(GTK_GRID(grid), logo, 0, 0, 2, 1);

    // Room selection
    room_label = gtk_label_new("Select Room:");
    gtk_grid_attach(GTK_GRID(grid), room_label, 0, 1, 1, 1);

    room_dropdown = gtk_combo_box_text_new();
    for (int i = 0; i < MAX_ROOMS; i++) {
        char room_text[10];
        snprintf(room_text, sizeof(room_text), "%d", rooms[i].room_number);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(room_dropdown), room_text);
    }
    gtk_grid_attach(GTK_GRID(grid), room_dropdown, 1, 1, 1, 1);

    // Name entry
    name_label = gtk_label_new("Your Name:");
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 2, 1, 1);

    name_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), name_entry, 1, 2, 1, 1);

    // Phone entry
    phone_label = gtk_label_new("Phone Number:");
    gtk_grid_attach(GTK_GRID(grid), phone_label, 0, 3, 1, 1);

    phone_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), phone_entry, 1, 3, 1, 1);

    // Book button
    book_button = gtk_button_new_with_label("Book Room");
    gtk_grid_attach(GTK_GRID(grid), book_button, 0, 4, 2, 1);

    // Connect button callback
    widgets[0] = room_dropdown;
    widgets[1] = name_entry;
    widgets[2] = phone_entry;
    g_signal_connect(book_button, "clicked", G_CALLBACK(on_book_button_clicked), widgets);

    gtk_widget_show_all(window);
}

// Main function
int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.example.room_booking", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
