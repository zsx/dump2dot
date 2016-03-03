#include "cmd_parse.h"
#include "dump.h"
#include <cstdlib>

#include <gtk/gtk.h>

#include <iostream>

struct App {
    cmd_opt opt;
    MemoryDump dump;
    bool imported;
    GtkWidget *input_file_button;
    GtkWidget *output_file_button;
    GtkWidget *threshold_entry;
    GtkWidget *depth_button;
    GtkWidget *node_entry;
    GtkWidget *label_entry;
} app;

extern "C" {

    void import_button_clicked_cb(GtkButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        std::cout << "importing ..." << std::endl;
        if (!app->dump.import(app->opt.ifile)) {
            std::cout << "Failed to parse the input" << std::endl;
        }
        app->dump.update_subtree_size();
        app->imported = true;
        std::cout << "imported from '" << app->opt.ifile << "'" << std::endl;
    }

    void write_button_clicked_cb(GtkButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        if (!app->imported) {
            import_button_clicked_cb(NULL, user_data);
        }
        std::cout << "writing ..." << std::endl;
        const char *text = gtk_entry_get_text(GTK_ENTRY(app->threshold_entry));
        char *p = const_cast<char*>(text);
        double threshold = std::strtod(text, &p);
        if (threshold > 1 || threshold < 0) {
            std::cout << "threshold must between 0 and 1" << std::endl;
            return;
        }
        app->opt.threshold = threshold;
        app->opt.depth = static_cast<int>(gtk_spin_button_get_value(GTK_SPIN_BUTTON(app->depth_button)));

        app->opt.nodes.clear();
        auto buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->node_entry));
        int n = gtk_text_buffer_get_line_count(buf);
        for (int i = 0; i < n; i++) {
            GtkTextIter start, end;
            gtk_text_buffer_get_iter_at_line(buf, &start, i);
            end = start;
            gtk_text_iter_forward_to_line_end(&end);

            char* text = gtk_text_buffer_get_slice(buf, &start, &end, FALSE);
            app->opt.parse_node(text);
            g_free(text);
        }

        std::cout << "parsing labels" << std::endl;
        app->opt.labels.clear();
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->label_entry));
        n = gtk_text_buffer_get_line_count(buf);
        for (int i = 0; i < n; i++) {
            GtkTextIter start, end;
            gtk_text_buffer_get_iter_at_line(buf, &start, i);
            end = start;
            gtk_text_iter_forward_to_line_end(&end);

            char* text = gtk_text_buffer_get_slice(buf, &start, &end, FALSE);
            std::cout << "label: '" << text << "'" << std::endl;
            if (text != nullptr && *text != '\0') {
                bool dec = true;
                if (text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) dec = false;
                long long i;
                if (dec) {
                    i = std::stoll(text, NULL, 10);
                }
                else {
                    i = std::stoll(&text[2], NULL, 16);
                }
                app->opt.labels.push_back(i);
            }
            g_free(text);
        }
        app->dump.write_output(app->opt);
        std::cout << "exported to '" << app->opt.ofile << "'" << std::endl;
    }

    void input_file_changed(GtkButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app->input_file_button));
        if (path != NULL) {
            app->opt.ifile = path;
            g_free(path);
            app->imported = false;
        }
        std::cout << "Input file changed to '" << app->opt.ifile << "'" << std::endl;
    }

    void output_file_changed(GtkButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(app->output_file_button));
        if (path != NULL) {
            app->opt.ofile = path;
            g_free(path);
        }
        std::cout << "Output file changed to '" << app->opt.ofile << "'" << std::endl;
    }

    void critical_button_toggled_cb(GtkToggleButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        app->opt.critical_only = gtk_toggle_button_get_active(button);
    }

    void dot_button_toggled_cb(GtkToggleButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        if (gtk_toggle_button_get_active(button)) {
            app->opt.export_type = EXPORT_DOT;
        }
    }
    void gml_button_toggled_cb(GtkToggleButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        if (gtk_toggle_button_get_active(button)) {
            app->opt.export_type = EXPORT_GML;
        }
    }
    void graphml_button_toggled_cb(GtkToggleButton *button, void *user_data)
    {
        App *app = static_cast<App*>(user_data);
        if (gtk_toggle_button_get_active(button)) {
            app->opt.export_type = EXPORT_GRAPHML;
        }
    }
}

#ifdef WIN32
#include <Windows.h>
int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
    )
#else
int main (int argc, char **argv)
#endif
{
    GtkWidget *win = NULL;
    GtkBuilder *builder;

    gtk_init(&argc, &argv);

    try {
        auto ret = app.opt.parse(argc, argv);
        if (ret < 0) {
            std::cout << "Wrong command line" << std::endl;
            std::cout << app.opt.help(argv[0]);
            //			return EXIT_FAILURE;
        }
        else if (ret > 0) {
            std::cout << app.opt.help(argv[0]);
            return EXIT_SUCCESS;
        }
    }
    catch (...) {
        std::cout << "Exception happened in command parse" << std::endl;
        app.opt.help(argv[0]);
        return EXIT_FAILURE;
    }

    std::cout.imbue(std::locale(""));

    app.imported = false;

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "dump2dot.glade", NULL);

    /* Connect signals */
    gtk_builder_connect_signals(builder, &app);

    app.input_file_button = GTK_WIDGET(gtk_builder_get_object(builder, "input_file_button"));
    app.output_file_button = GTK_WIDGET(gtk_builder_get_object(builder, "output_file_button"));
    app.depth_button = GTK_WIDGET(gtk_builder_get_object(builder, "depth_spin_button"));
    app.threshold_entry = GTK_WIDGET(gtk_builder_get_object(builder, "threshold_entry"));
    app.node_entry = GTK_WIDGET(gtk_builder_get_object(builder, "node_entry"));
    app.label_entry = GTK_WIDGET(gtk_builder_get_object(builder, "label_entry"));

    GtkAdjustment *adj = gtk_adjustment_new(-1, -1, 999999999, 1, 10, 1);
    gtk_spin_button_configure(GTK_SPIN_BUTTON(app.depth_button), adj, 1, 0);

    //GtkFileChooser *input_chooser = GTK_FILE_CHOOSER(input_chooser_button);
//    gtk_file_chooser_set_current_folder_file(app.opt.ifile);

    win = GTK_WIDGET(gtk_builder_get_object(builder, "dialog1"));

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(G_OBJECT(builder));

    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}
