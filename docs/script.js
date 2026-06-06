// Javascript Functionality: Numerical Method Solver Documentation

document.addEventListener('DOMContentLoaded', () => {
    
    // --- 1. Responsive Navbar Drawer ---
    const navToggle = document.getElementById('nav-toggle');
    const navMenu = document.querySelector('.nav-menu');
    
    if (navToggle && navMenu) {
        navToggle.addEventListener('click', () => {
            navMenu.classList.toggle('open');
            const icon = navToggle.querySelector('i');
            if (navMenu.classList.contains('open')) {
                icon.className = 'fa-solid fa-xmark';
            } else {
                icon.className = 'fa-solid fa-bars';
            }
        });
        
        // Close menu when clicking a link
        document.querySelectorAll('.nav-link').forEach(link => {
            link.addEventListener('click', () => {
                navMenu.classList.remove('open');
                const icon = navToggle.querySelector('i');
                if (icon) icon.className = 'fa-solid fa-bars';
            });
        });
    }

    // --- 2. Navbar Scroll Effect ---
    const navbar = document.getElementById('navbar');
    window.addEventListener('scroll', () => {
        if (window.scrollY > 50) {
            navbar.classList.add('scrolled');
        } else {
            navbar.classList.remove('scrolled');
        }
    });

    // --- 3. Interactive Method Switcher ---
    const tabBtns = document.querySelectorAll('.tab-btn');
    const methodPanes = document.querySelectorAll('.method-pane');

    tabBtns.forEach(btn => {
        btn.addEventListener('click', () => {
            const targetMethod = btn.getAttribute('data-method');
            
            // Toggle tab buttons
            tabBtns.forEach(b => b.classList.remove('active'));
            btn.classList.add('active');
            
            // Toggle content panes
            methodPanes.forEach(pane => {
                pane.classList.remove('active');
                if (pane.id === `pane-${targetMethod}`) {
                    pane.classList.add('active');
                }
            });
        });
    });

    // --- 4. Live Search Filter for Cheatsheet ---
    const searchInput = document.getElementById('cheatsheet-search');
    const cheatsheetTableRows = document.querySelectorAll('#cheatsheet-table tbody tr');

    if (searchInput) {
        searchInput.addEventListener('input', (e) => {
            const query = e.target.value.toLowerCase().trim();
            
            cheatsheetTableRows.forEach(row => {
                const searchKeywords = row.getAttribute('data-search').toLowerCase();
                
                if (query === '' || searchKeywords.includes(query)) {
                    row.style.display = '';
                    row.style.opacity = '1';
                } else {
                    row.style.display = 'none';
                    row.style.opacity = '0';
                }
            });
        });
    }

    // --- 5. ScrollSpy (Navbar and Sidebar highlighting) ---
    const sections = document.querySelectorAll('section[id], main section[id]');
    const navLinks = document.querySelectorAll('.nav-link');
    const scrollspyLinks = document.querySelectorAll('.scrollspy-link');

    function highlightNavigation() {
        const scrollPosition = window.scrollY + 200; // Offset for detection

        sections.forEach(section => {
            const sectionTop = section.offsetTop;
            const sectionHeight = section.offsetHeight;
            const sectionId = section.getAttribute('id');

            if (scrollPosition >= sectionTop && scrollPosition < sectionTop + sectionHeight) {
                // Highlight navbar link
                navLinks.forEach(link => {
                    link.classList.remove('active');
                    if (link.getAttribute('href') === `#${sectionId}`) {
                        link.classList.add('active');
                    }
                });

                // Highlight sidebar scrollspy link
                scrollspyLinks.forEach(link => {
                    link.classList.remove('active');
                    if (link.getAttribute('href') === `#${sectionId}`) {
                        link.classList.add('active');
                    }
                });
            }
        });
    }

    window.addEventListener('scroll', highlightNavigation);
    highlightNavigation(); // Run once on load
});
