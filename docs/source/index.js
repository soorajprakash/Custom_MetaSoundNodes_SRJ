const fs = require('fs');
const path = require('path');

const inputFile = path.join(__dirname, 'nodes.json');
const outputDir = path.join(__dirname, '../');

if (!fs.existsSync(outputDir)) {
  fs.mkdirSync(outputDir);
}

const data = JSON.parse(fs.readFileSync(inputFile, 'utf8'));

const sidebarContent = data.map(d => {
    const safeName = d.name.replace(/\s+/g, '');
    const nodeFileName = `${safeName}.html`;
    return `<li><a href="${nodeFileName}">${d.name}</a></li>`;
}).join('\n');


data.forEach(node => {
  const { name, description, image, inputs, outputs } = node;

  const inputRows = inputs.map(input => {
    return `
      <tr>
        <td>${input.name}</td>
        <td>${input.description}</td>
        <td>${input.type}</td>
      </tr>
    `;
  }).join('\n');

  const outputRows = outputs.map(output => {
    return `
      <tr>
        <td>${output.name}</td>
        <td>${output.description}</td>
        <td>${output.type}</td>
      </tr>
    `;
  }).join('\n');

  
  const htmlContent = `<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>${name}</title>
  <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
  <link rel="stylesheet" href="./style.css">
</head>
<body>
    <nav class="sidebar">
        <ul>
            ${sidebarContent}
        </ul>
    </nav>
  <main>
    <h1>${name}</h1>
    <img src="./svg/${image}" alt="${name}">
    <p>${description}</p>
    <h2>Inputs</h2>
    <table>
      <thead>
        <tr>
          <th>Name</th>
          <th>Description</th>
          <th>Type</th>
        </tr>
      </thead>
      <tbody>
        ${inputRows}
      </tbody>
    </table>

    <h2>Outputs</h2>
    <table>
      <thead>
        <tr>
          <th>Name</th>
          <th>Description</th>
          <th>Type</th>
        </tr>
      </thead>
      <tbody>
        ${outputRows}
      </tbody>
    </table>
  </main>
  <button class="menu-toggle" aria-label="Toggle Menu">☰</button>
</body>

<script>
  document.addEventListener('DOMContentLoaded', function() {
    const sidebar = document.querySelector('.sidebar');
    const toggleBtn = document.querySelector('.menu-toggle');

    toggleBtn.addEventListener('click', () => {
      sidebar.classList.toggle('show');
    });
  });
</script>
</html>
`;

  const fileName = name.replace(/\s+/g, '') + '.html';
  const filePath = path.join(outputDir, fileName);
  
  fs.writeFileSync(filePath, htmlContent, 'utf8');
  console.log(`- ${fileName}`);
});