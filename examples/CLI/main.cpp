#include "include/Qt6MathJax.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include <iostream>
#include <thread>
#include <chrono>

void setupDebug( int debugLevel, int port )
{
    std::vector< std::pair< QString, bool > > catVector =   //
        {
            { "js", false },   //
            { "Towel42.Qt6MathJax", false },
            { "Towel42.Qt6MathJax.Console", false },
            { "Towel42.Qt6MathJax.Debug", false },
            { "Towel42.Qt6MathJax.QRC", false }   //
        };
    if ( debugLevel > 0 )
    {
        NTowel42::CQt6MathJax::enableDebugConsole( port );
        catVector[ 0 ].second = true;
        catVector[ 1 ].second = true;
    }

    if ( debugLevel > 1 )
    {
        catVector[ 2 ].second = true;
    }

    if ( debugLevel > 2 )
    {
        catVector[ 3 ].second = true;
    }

    if ( debugLevel > 3 )
    {
        catVector[ 4 ].second = true;
    }

    QStringList categories;
    for ( auto &&ii : catVector )
    {
        categories << QString( "%1=%2" ).arg( ii.first ).arg( ii.second ? "true" : "false" );
    }

    QLoggingCategory::setFilterRules( categories.join( "\n" ) );
}

int main( int argc, char *argv[] )
{
    QApplication appl( argc, argv );

    QCoreApplication::setApplicationName( "Qt6MathJax-CLIExample" );
    QCoreApplication::setApplicationVersion( "1.0" );

    QCommandLineParser parser;
    parser.setApplicationDescription( "Qt6MathJax-CLIExample - A simple tool to convert TeX strings into SVG." );
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();

    auto inputTeXOption = QCommandLineOption( QStringList() << "input" << "i", "The input TeX to be rendered.", "TeX" );
    parser.addOption( inputTeXOption );

    auto outputFileOption = QCommandLineOption( QStringList() << "output" << "o", "Output SVG file (svg extension will be added if missing) <default=output.svg>", "output file", "output.svg" );
    parser.addOption( outputFileOption );

    auto debugOption = QCommandLineOption(
        QStringList() << "debug" << "d",
        "Debug level <default=0>\n"
        "1 - Enables the Chromium Debug System at the port set via -port, as well as the 'js', 'Towel42.Qt6MathJax' logging categories\n"
        "2 - Enables level 1 and the Towel42.Qt6MathJax.Console logging category\n"
        "3 - Enables level 2 and the Towel42.Qt6MathJax.Debug\n"
        "4 - Enables level 2 and the Towel42.Qt6MathJax.QRC",
        "level", "0" );
    parser.addOption( debugOption );

    auto debugPortOption = QCommandLineOption( QStringList() << "port" << "p", "Debug Port <default=12345>", "port", "12345" );
    parser.addOption( debugPortOption );

    parser.process( appl );
    if ( !parser.unknownOptionNames().isEmpty() )
    {
        std::cerr << "The following options were set and are unknown:\n";
        for ( auto &&ii : parser.unknownOptionNames() )
            std::cerr << "    " << ii.toStdString() << "\n";
        parser.showHelp();
        return -1;
    }

    if ( parser.isSet( helpOption ) )
    {
        parser.showHelp();
        return 0;
    }

    if ( parser.isSet( versionOption ) )
    {
        parser.showVersion();
        return 0;
    }

    auto outputFile = parser.value( outputFileOption );

    if ( QFileInfo( outputFile ).suffix() != "svg" )
        outputFile += ".svg";

    if ( !parser.isSet( inputTeXOption ) )
    {
        parser.showHelp();
        return -1;
    }

    bool aOK = false;
    auto debugLevel = parser.value( debugOption ).toInt( &aOK );
    if ( !aOK )
    {
        std::cerr << "Could not process debug level '" << parser.value( debugOption ).toStdString() << "' must be an integer\n";
        return -1;
    }

    auto port = parser.value( debugPortOption ).toInt( &aOK );
    if ( !aOK || ( ( port < 0 ) || ( port > 65535 ) ) )
    {
        std::cerr << "Could not process debug port '" << parser.value( debugPortOption ).toStdString() << "' must be an integer between 0 and 65535\n";
        return -1;
    }
    setupDebug( debugLevel, port );

    auto inputTeX = parser.value( inputTeXOption );
    int retVal = -1;
    std::cout << "Rendering TeX input: '" << inputTeX.toStdString() << "'" << "\n";
    NTowel42::CQt6MathJax engine;
    engine.renderSVG(
        inputTeX,   //
        [ &engine, outputFile, &retVal ]( const std::optional< QByteArray > &svg )   //
        {
            if ( !svg.has_value() )
            {
                std::cerr << "ERROR: Problem rendering TeX: see previous errors.\n";
                retVal = -1;
                return;
            }

            std::cout << "Finished rendering\n";

            QFile outfile( outputFile );
            if ( !outfile.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
            {
                std::cerr << "ERROR: Could not open file '" + outputFile.toStdString() + "'." << "\n";
                retVal = -2;
                return;
            }

            outfile.write( svg.value() );
            std::cout << "Finished writing file '" << outputFile.toStdString() << "'." << "\n";
            retVal = 0;
        },
        [ = ]( const QString &msg )
        {
            std::cerr << "ERROR: " << msg.toStdString() << "\n";
        }   //
    );

    return retVal;
}
