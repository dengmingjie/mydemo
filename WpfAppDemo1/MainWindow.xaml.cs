using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfAppDemo1
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button1_Click(object sender, RoutedEventArgs e)
        {
            MessageBox.Show(this.TextBox1.Text + " Hello!!!", "Tip");
        }

        private void Button2_Click(object sender, RoutedEventArgs e)
        {
            Thread t = new Thread(delegate ()
            {
                //this.TextBox1.Text = "非UI线程修改界面元素";
                //Application.Current.Dispatcher.Invoke(() =>
                //{
                //    this.TextBox1.Text = "非UI线程修改界面元素";
                //});
                this.TextBox1.Dispatcher.Invoke(new Action(delegate () {

                }));
            });
            t.Start();
        }
    }
}
