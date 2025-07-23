namespace RainforestControlTool;

public partial class App : Application
{
    /// <summary>
    /// DI provider
    /// </summary>
    public static IServiceProvider ServiceProvider;
    
    public App(IServiceProvider serviceProvider)
    {
        ServiceProvider = serviceProvider;
        
        InitializeComponent();
    }

    protected override Window CreateWindow(IActivationState? activationState)
    {
        return new Window(new AppShell());
    }
}