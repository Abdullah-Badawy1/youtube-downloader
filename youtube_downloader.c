#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Function to execute yt-dlp and return output with error handling
char *get_yt_dlp_output(const char *command, GtkWidget *status_label) {
    FILE *fp;
    char buffer[1024];
    char *output = NULL;
    size_t output_len = 0;

    fp = popen(command, "r");
    if (fp == NULL) {
        gtk_label_set_text(GTK_LABEL(status_label), "Error: Failed to execute yt-dlp.");
        return NULL;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t buffer_len = strlen(buffer);
        char *new_output = realloc(output, output_len + buffer_len + 1);
        if (new_output == NULL) {
            gtk_label_set_text(GTK_LABEL(status_label), "Error: Memory allocation failed.");
            free(output);
            pclose(fp);
            return NULL;
        }
        output = new_output;
        strcpy(output + output_len, buffer);
        output_len += buffer_len;
    }

    if (output) {
        output[output_len] = '\0';
    }

    pclose(fp);
    return output;
}

// Callback for the "Get Formats" button
static void on_get_formats_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **widgets = (GtkWidget **)data;
    GtkWidget *entry = widgets[0];
    GtkWidget *text_view = widgets[1];
    GtkWidget *status_label = widgets[3]; // Access the status label

    const char *url = gtk_entry_get_text(GTK_ENTRY(entry));

    if (strlen(url) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Please enter a valid URL.");
        return;
    }

    char command[512];
    snprintf(command, sizeof(command), "yt-dlp -F '%s'", url);

    gtk_label_set_text(GTK_LABEL(status_label), "Fetching formats...");

    char *output = get_yt_dlp_output(command, status_label);
    if (output == NULL) {
        // Error message is already set in get_yt_dlp_output
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, output, -1);
    free(output);

    gtk_label_set_text(GTK_LABEL(status_label), "Formats fetched successfully.");
}

// Callback for the "Download" button
static void on_download_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **widgets = (GtkWidget **)data;
    GtkWidget *entry = widgets[0];
    GtkWidget *format_entry = widgets[2];
    GtkWidget *status_label = widgets[3];

    const char *url = gtk_entry_get_text(GTK_ENTRY(entry));
    const char *format = gtk_entry_get_text(GTK_ENTRY(format_entry));

    if (strlen(url) == 0 || strlen(format) == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "URL or format is missing.");
        return;
    }

    for (size_t i = 0; i < strlen(format); ++i) {
        if (!isdigit(format[i])) {
            gtk_label_set_text(GTK_LABEL(status_label), "Invalid format code. It must be numeric.");
            return;
        }
    }

    char command[512];
    snprintf(command, sizeof(command), "yt-dlp -f %s '%s' -o 'downloads/%%(title)s.%%(ext)s'", format, url);

    gtk_label_set_text(GTK_LABEL(status_label), "Downloading...");

    int ret = system(command);
    if (ret == 0) {
        gtk_label_set_text(GTK_LABEL(status_label), "Download completed successfully!");
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Download failed. Check the format or URL.");
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "YouTube Downloader");
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *url_label = gtk_label_new("Enter YouTube URL:");
    gtk_grid_attach(GTK_GRID(grid), url_label, 0, 0, 1, 1);

    GtkWidget *url_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), url_entry, 1, 0, 2, 1);

    GtkWidget *get_formats_button = gtk_button_new_with_label("Get Formats");
    gtk_grid_attach(GTK_GRID(grid), get_formats_button, 3, 0, 1, 1);

    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled_window, 780, 300);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 1, 4, 1);

    GtkWidget *format_label = gtk_label_new("Enter Format Code:");
    gtk_grid_attach(GTK_GRID(grid), format_label, 0, 2, 1, 1);

    GtkWidget *format_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), format_entry, 1, 2, 2, 1);

    GtkWidget *download_button = gtk_button_new_with_label("Download");
    gtk_grid_attach(GTK_GRID(grid), download_button, 3, 2, 1, 1);

    GtkWidget *status_label = gtk_label_new("Status: Ready");
    gtk_grid_attach(GTK_GRID(grid), status_label, 0, 3, 4, 1);

    GtkWidget *widgets[] = {url_entry, text_view, format_entry, status_label};

    g_signal_connect(get_formats_button, "clicked", G_CALLBACK(on_get_formats_clicked), widgets);
    g_signal_connect(download_button, "clicked", G_CALLBACK(on_download_clicked), widgets);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
