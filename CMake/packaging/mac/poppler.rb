# This file contains a formula for installing Poppler on Mac OS X using the
# Homebrew package manager:
#
#     http://brew.sh/
#
# To install Poppler using this formula:
#
#     brew install path/to/this/poppler.rb
#
# Changes compared to Homebrew's standard Poppler formula:
#
#   - TeXworks-specific patches are applied to
#        - help Qt apps find the poppler-data directory.
#        - use native Mac OS X font handling (instead of fontconfig)
class Poppler < Formula
  desc "PDF rendering library (based on the xpdf-3.0 code base)"
  homepage "https://poppler.freedesktop.org/"
  url "https://poppler.freedesktop.org/poppler-0.54.0.tar.xz"
  sha256 "10cca9a67cc2e6f4f9024192b1067c444218bf94430891f43dc819d78536ca31"

# BEGIN TEXWORKS MODIFICATION
#  bottle do
#    rebuild 1
#    sha256 "5b6de69e6ab1996332934e21b3a5220990e2a5943775d002ee8e5c2447b93758" => :sierra
#    sha256 "44fcbb473c78c5ce034b65885fcb2e05d4ef0b2d537a043b7d2b6d44d075907b" => :el_capitan
#    sha256 "47787f7ffa8f69dea50ebb1391186d8146386ad46f2f6b69e3786b7271caeb9d" => :yosemite
#  end
  version '0.54.0-texworks'

  TEXWORKS_SOURCE_DIR = Pathname.new(__FILE__).realpath.dirname.join('../../..')
  TEXWORKS_PATCH_DIR = TEXWORKS_SOURCE_DIR + 'lib-patches/'
  patch do
    url "file://" + TEXWORKS_PATCH_DIR + 'poppler-0001-Fix-bogus-memory-allocation-in-SplashFTFont-makeGlyp.patch'
    sha256 "3b40b066995756a0c80badfe47e701bb0438305f3b8eb15b67875c7de38c7290"
  end
  patch do
    url "file://" + TEXWORKS_PATCH_DIR + 'poppler-0002-Native-Mac-font-handling.patch'
    sha256 "41cedfe209c203833574ace4c60c0440840cb03f7ba687a27a3a350b0d868cc4"
  end
  patch do
    url "file://" + TEXWORKS_PATCH_DIR + 'poppler-0003-Add-support-for-persistent-GlobalParams.patch'
    sha256 "6c17fe4d91c7c5d77e265af48c511db31fce73370cd2af4cbacc218435c9c86a"
  end

  depends_on "autoconf" => :build
  depends_on "automake" => :build
  depends_on "libtool" => :build
# END TEXWORKS MODIFICATION

  option "with-qt", "Build Qt5 backend"
  option "with-little-cms2", "Use color management system"

  deprecated_option "with-qt4" => "with-qt"
  deprecated_option "with-qt5" => "with-qt"
  deprecated_option "with-lcms2" => "with-little-cms2"

  depends_on "pkg-config" => :build
  depends_on "cairo"
  depends_on "fontconfig"
  depends_on "freetype"
  depends_on "gettext"
  depends_on "glib"
  depends_on "gobject-introspection"
  depends_on "jpeg"
  depends_on "libpng"
  depends_on "libtiff"
  depends_on "openjpeg"
  depends_on "qt" => :optional
  depends_on "little-cms2" => :optional

  conflicts_with "pdftohtml", "pdf2image", "xpdf",
    :because => "poppler, pdftohtml, pdf2image, and xpdf install conflicting executables"

  resource "font-data" do
    url "https://poppler.freedesktop.org/poppler-data-0.4.7.tar.gz"
    sha256 "e752b0d88a7aba54574152143e7bf76436a7ef51977c55d6bd9a48dccde3a7de"
  end

  needs :cxx11 if build.with?("qt") || MacOS.version < :mavericks

  def install
    ENV.cxx11 if build.with?("qt") || MacOS.version < :mavericks
    ENV["LIBOPENJPEG_CFLAGS"] = "-I#{Formula["openjpeg"].opt_include}/openjpeg-2.1"

    args = %W[
      --disable-dependency-tracking
      --prefix=#{prefix}
      --enable-xpdf-headers
      --enable-poppler-glib
      --disable-gtk-test
      --enable-introspection=yes
      --disable-poppler-qt4
    ]

    if build.with? "qt"
      args << "--enable-poppler-qt5"
    else
      args << "--disable-poppler-qt5"
    end

    args << "--enable-cms=lcms2" if build.with? "little-cms2"

# BEGIN TEXWORKS MODIFICATION
    # We changed the config file (to add native font handling), so we need to
    # update the configure script
    system "autoreconf", "-ivf"
# END TEXWORKS MODIFICATION

    system "./configure", *args
    system "make", "install"
    resource("font-data").stage do
      system "make", "install", "prefix=#{prefix}"
    end
  end

  test do
    system "#{bin}/pdfinfo", test_fixtures("test.pdf")
  end
end
