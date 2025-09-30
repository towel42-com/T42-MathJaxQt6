#include "include/Qt6MathJax.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include <iostream>

int main( int argc, char *argv[] )
{
    QApplication appl( argc, argv );
    QCoreApplication::setApplicationName( "Qt6MathJax-CLIExample" );
    QCoreApplication::setApplicationVersion( "1.0" );

    QLoggingCategory::setFilterRules( ( QStringList() << "Qt6MathJax.*=false" ).join( "\n" ) );

    QCommandLineParser parser;
    parser.setApplicationDescription( "Qt6MathJax-CLIExample - A simple tool to convert TeX strings into SVG." );
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();

    auto inputTeXOption = QCommandLineOption( QStringList() << "input" << "i", "The input TeX to be rendered.", "TeX" );
    parser.addOption( inputTeXOption );

    auto outputFileOption = QCommandLineOption( QStringList() << "output" << "o", "Output SVG file (svg extension will be added if missing) <default=output.svg>", "output file", "output.svg" );
    parser.addOption( outputFileOption );

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

    auto inputTeX = parser.value( inputTeXOption );
    int retVal = -1;
    std::cout << "Rendering TeX input: '" << inputTeX.toStdString() << "'" << "\n";
    NTowel42::CQt6MathJax engine;
    engine.renderSVG(
        inputTeX,   //
        [ &engine, outputFile, &retVal ]( const std::optional< QByteArray > &svg )   //
        {
            std::cout << "Finished rendering\n";
            if ( !svg.has_value() )
            {
                std::cerr << "ERROR: Problem rendering Tex: " + engine.errorMessage().toStdString() << "\n";
                retVal = -1;
                return;
            }

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
        } );

    return retVal;
}
