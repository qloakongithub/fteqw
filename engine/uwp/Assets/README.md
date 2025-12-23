# UWP Assets

This directory should contain the following image assets for the UWP package:

- **Logo.png** - 150x150 pixels - Square tile logo
- **SmallLogo.png** - 44x44 pixels - Small tile logo
- **StoreLogo.png** - 50x50 pixels - Store logo
- **SplashScreen.png** - 620x300 pixels - Splash screen
- **WideLogo.png** - 310x150 pixels - Wide tile logo (optional)

## Creating Assets

These assets should follow the [UWP tile asset guidelines](https://docs.microsoft.com/en-us/windows/uwp/design/style/app-icons-and-logos).

For development, you can use placeholder images. For production:

1. Create high-quality PNG images at the specified dimensions
2. Use transparent backgrounds where appropriate
3. Follow Microsoft Store branding guidelines
4. Include the FTEQW logo or branding

## Temporary Placeholder

During development, Visual Studio can generate temporary placeholder assets.
Replace these with proper artwork before release.

## Asset Requirements

UWP requires specific asset sizes for different contexts:
- **App list icon**: 44x44px (SmallLogo.png)
- **Medium tile**: 150x150px (Logo.png)
- **Wide tile**: 310x150px (WideLogo.png - optional)
- **Splash screen**: 620x300px (SplashScreen.png)
- **Store logo**: 50x50px (StoreLogo.png)

All images should be PNG format with transparent backgrounds where suitable.
